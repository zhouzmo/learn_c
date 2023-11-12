#include <linux/module.h>
#include <linux/poll.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/slab.h>
#include <linux/fcntl.h>

struct gpio_key{
	int gpio;
	struct gpio_desc *gpiod;
	int flag;
	int irq;
} ;

static struct gpio_key *gpio_keys_100ask;

/* 主设备号                                                                 */
static int major = 0;
static struct class *gpio_key_class;

/* 环形缓冲区 */
#define BUF_LEN 128
static int g_keys[BUF_LEN];
static int r, w;

struct fasync_struct *button_fasync;

#define NEXT_POS(x) ((x+1) % BUF_LEN)

static int is_key_buf_empty(void)
{
	return (r == w);
}

static int is_key_buf_full(void)
{
	return (r == NEXT_POS(w));
}

static void put_key(int key)
{
	if (!is_key_buf_full())
	{
		g_keys[w] = key;
		w = NEXT_POS(w);
	}
}

static int get_key(void)
{
	int key = 0;
	if (!is_key_buf_empty())
	{
		key = g_keys[r];
		r = NEXT_POS(r);
	}
	return key;
}



static DECLARE_WAIT_QUEUE_HEAD(gpio_key_wait);

/*== 5、在.read 函数指针内部实现，等待数据`wait_event_interruptible`，若无数据，休眠  ==*/
static ssize_t gpio_key_drv_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	//printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	int err;
	int key;
	
	wait_event_interruptible(gpio_key_wait, !is_key_buf_empty());
	key = get_key();
	err = copy_to_user(buf, &key, 4);
	
	return 4;
}

/*== 7、在.poll 函数指针内部实现，等待数据 `poll_wait`，将事件放入wq  ==*/
static unsigned int gpio_key_drv_poll(struct file *fp, poll_table * wait)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	poll_wait(fp, &gpio_key_wait, wait);
	return is_key_buf_empty() ? 0 : POLLIN | POLLRDNORM;
}

/*== 8、在.fasync 函数指针内部实现，当 file 的 FASYNC 位发生变化时
	该函数会变调用，检查标志位  ==*/
// fcntl(fd, F_SETFL, flags | FASYNC);  
// 如果 FASYNC 被设置，int on 将被设置为非零值，允许进程注册对文件的异步通知。
static int gpio_key_drv_fasync(int fd, struct file *file, int on)
{

	/*
	int fasync_helper(int fd, struct file *file, int on, struct fasync_struct **fapp);

	参数：
	- `fd`：表示文件描述符，用于标识要进行异步通知管理的文件。
	- `file`：指向表示文件的结构体的指针。
	- `on`：一个整数标志，指示是否要开启（非零值）或关闭（零值）异步通知。
	- `fapp`：一个指向指针的指针，用于接收指向 fasync_struct 结构体的指针，该结构体用于管理异步通知。

	作用：
	- `fasync_helper` 函数用于管理文件的异步通知。
	当 `on` 参数为非零值时，它允许进程注册对文件的异步通知，
	而当 `on` 参数为零值时，它取消之前的异步通知注册。
	这样，当文件状态发生变化时，内核将通知已注册的进程，而无需进程轮询文件状态。
	这对于需要实时响应文件状态变化的应用程序非常有用。

	当函数成功执行时，返回值大于等于 0，表示操作成功。
	当函数执行失败时，返回值小于 0，表示操作失败，通常会返回 -EIO。

	这个函数通常用于驱动程序中，以便允许用户空间进程注册对设备文件的异步通知。
	*/
	// 检查设置 flags 是否成功
	if (fasync_helper(fd, file, on, &button_fasync) >= 0)
		return 0;
	else
		return -EIO;
}


/*== 1、实现 file_operations 结构体的 read、poll 、fasync等函数，向 app 提供对应接口 ==*/
static struct file_operations gpio_key_drv = {
	.owner	 = THIS_MODULE,
	.read    = gpio_key_drv_read,
	.poll    = gpio_key_drv_poll,
	.fasync  = gpio_key_drv_fasync,
};


/*== 6、在中断函数 handle 实现，硬件数据的获取，保存，
等待队列的唤醒 `wake_up_interruptible`  
sync 信号发送 kill_fasync ==*/
static irqreturn_t gpio_key_isr(int irq, void *dev_id)
{
	struct gpio_key *gpio_key = dev_id;
	int val;
	int key;
	
	val = gpiod_get_value(gpio_key->gpiod);
	

	printk("key %d %d\n", gpio_key->gpio, val);
	key = (gpio_key->gpio << 8) | val;
	put_key(key);
	wake_up_interruptible(&gpio_key_wait);
	kill_fasync(&button_fasync, SIGIO, POLL_IN);
	
	return IRQ_HANDLED;
}

/*== 4、在.probe 函数指针内部实现，设备树信息的获取保存，中断的注册，类和设备的创建  ==*/
static int gpio_key_probe(struct platform_device *pdev)
{
	int err;
	struct device_node *node = pdev->dev.of_node;
	int count;
	int i;
	enum of_gpio_flags flag;
		
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

	count = of_gpio_count(node);
	if (!count)
	{
		printk("%s %s line %d, there isn't any gpio available\n", __FILE__, __FUNCTION__, __LINE__);
		return -1;
	}

	gpio_keys_100ask = kzalloc(sizeof(struct gpio_key) * count, GFP_KERNEL);
	for (i = 0; i < count; i++)
	{
		gpio_keys_100ask[i].gpio = of_get_gpio_flags(node, i, &flag);
		if (gpio_keys_100ask[i].gpio < 0)
		{
			printk("%s %s line %d, of_get_gpio_flags fail\n", __FILE__, __FUNCTION__, __LINE__);
			return -1;
		}
		gpio_keys_100ask[i].gpiod = gpio_to_desc(gpio_keys_100ask[i].gpio);
		gpio_keys_100ask[i].flag = flag & OF_GPIO_ACTIVE_LOW;
		gpio_keys_100ask[i].irq  = gpio_to_irq(gpio_keys_100ask[i].gpio);
	}

	for (i = 0; i < count; i++)
	{
		err = request_irq(gpio_keys_100ask[i].irq, gpio_key_isr, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "100ask_gpio_key", &gpio_keys_100ask[i]);
	}

	/* 注册file_operations 	*/
	major = register_chrdev(0, "100ask_gpio_key", &gpio_key_drv);  /* /dev/gpio_key */

	gpio_key_class = class_create(THIS_MODULE, "100ask_gpio_key_class");
	if (IS_ERR(gpio_key_class)) {
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		unregister_chrdev(major, "100ask_gpio_key");
		return PTR_ERR(gpio_key_class);
	}

	device_create(gpio_key_class, NULL, MKDEV(major, 0), NULL, "100ask_gpio_key"); /* /dev/100ask_gpio_key */
        
    return 0;
    
}

static int gpio_key_remove(struct platform_device *pdev)
{
	//int err;
	struct device_node *node = pdev->dev.of_node;
	int count;
	int i;

	device_destroy(gpio_key_class, MKDEV(major, 0));
	class_destroy(gpio_key_class);
	unregister_chrdev(major, "100ask_gpio_key");

	count = of_gpio_count(node);
	for (i = 0; i < count; i++)
	{
		free_irq(gpio_keys_100ask[i].irq, &gpio_keys_100ask[i]);
	}
	kfree(gpio_keys_100ask);
    return 0;
}


static const struct of_device_id ask100_keys[] = {
    { .compatible = "100ask,gpio_key" },
    { },
};

/*== 2、构建 platform_driver 结构体的 probe,remove 等函数函数指针，定义driver.of_match_table 硬件适配信息 ==*/
static struct platform_driver gpio_keys_driver = {
    .probe      = gpio_key_probe,
    .remove     = gpio_key_remove,
    .driver     = {
        .name   = "100ask_gpio_key",
        .of_match_table = ask100_keys,
    },
};

/*== 3、实现linux驱动框架的module_init/exit函数,调用 platform_driver_register，来匹配对应设备  ==*/
static int __init gpio_key_init(void)
{
    int err;
    
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	
    err = platform_driver_register(&gpio_keys_driver); 
	
	return err;
}


static void __exit gpio_key_exit(void)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

    platform_driver_unregister(&gpio_keys_driver);
}


/* 7. 其他完善：提供设备信息，自动创建设备节点                                     */

module_init(gpio_key_init);
module_exit(gpio_key_exit);

MODULE_LICENSE("GPL");


