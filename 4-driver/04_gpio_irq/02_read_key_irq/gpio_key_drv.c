#include <linux/module.h>

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


struct gpio_key{
	int gpio;
	struct gpio_desc *gpiod;
	int flag;
	int irq;
} ;

static struct gpio_key *gpio_keys_100ask;

static int major = 0;
static struct class *gpio_key_class;

static int g_key = 0;

static DECLARE_WAIT_QUEUE_HEAD(gpio_key_wait);

static ssize_t gpio_key_drv_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	// printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	int err;
	
	/*
	`wait_event_interruptible` 是 Linux 内核中用于实现等待事件的函数之一。
	它通常用于等待某个条件成立时才继续执行，而在等待期间允许当前进程被信号中断。以下是函数的一般形式：

	long wait_event_interruptible(wait_queue_head_t queue, condition);

	| 参数              | 描述                                                                 |
	| ----------------- | -------------------------------------------------------------------- |
	| `queue`           | 等待队列头，通常是一个 `wait_queue_head_t` 类型的变量或结构体。   |
	| `condition`       | 表示等待条件的表达式，通常是一个返回非零值或真的条件。          |

	**返回值**：返回值通常是一个长整数（`long`），表示等待过程的状态。如果返回正值，表示等待条件成立（成功）；
	如果返回负值，表示等待过程被信号中断。

	**函数说明**：`wait_event_interruptible` 用于使当前进程等待，直到条件 `condition` 变为真。
	在等待的过程中，进程将会进入可中断的睡眠状态，即可以被信号中断。一旦条件成立或者等待过程被信
	号中断，函数将返回相应的值。
	通常，`wait_event_interruptible` 会与其他函数一起使用，比如 `wake_up` 函数，以在某些事件发生时
	唤醒等待的进程。在你的代码示例中，`wait_event_interruptible(gpio_key_wait, g_key)` 表示当前进程
	将等待 `gpio_key_wait` 这个等待队列头上的事件，直到条件 `g_key` 变为真。如果条件满足或者等待过程
	被信号中断，函数将返回相应的值。这通常用于实现在某个条件满足时唤醒等待的进程。
	*/
	/*== 4、需要读数据时，检查是否有数据，没有则休眠 ==*/
	wait_event_interruptible(gpio_key_wait, g_key);
	err = copy_to_user(buf, &g_key, 4);
	g_key = 0;
	
	return 4;
}


static struct file_operations gpio_key_drv = {
	.owner	 = THIS_MODULE,
	.read    = gpio_key_drv_read,
};

/*== 3、实现中断函数 ==*/
static irqreturn_t gpio_key_isr(int irq, void *dev_id)
{
	struct gpio_key *gpio_key = dev_id;
	int val;
	val = gpiod_get_value(gpio_key->gpiod);
	

	printk("key %d %d\n", gpio_key->gpio, val);
	g_key = (gpio_key->gpio << 8) | val;
	/*== 5、按下按键，触发中断函数，唤醒队列中休眠线程 ==*/
	wake_up_interruptible(&gpio_key_wait);
	
	return IRQ_HANDLED;
}


static int gpio_key_probe(struct platform_device *pdev)
{
	int err;
	struct device_node *node = pdev->dev.of_node;
	int count;
	int i;
	enum of_gpio_flags flag;
		
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	/*== 1、获取设备树 GPIO ，保存 GPIO 信息==*/
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
		// 获取 gpio 的软件中断号 irq
		gpio_keys_100ask[i].irq  = gpio_to_irq(gpio_keys_100ask[i].gpio);
	}

	for (i = 0; i < count; i++)
	{	
		/*
		`request_threaded_irq` 是一个函数，通常在 Linux 内核编程中用于请求中断服务例程的注册。

		int request_threaded_irq(unsigned int irq, irq_handler_t handler, irq_handler_t thread_fn, unsigned long flags, const char *name, void *dev);

		| 参数      | 描述                                                                                                                         |
		| --------- | ---------------------------------------------------------------------------------------------------------------------------- |
		| `irq`     | 中断请求号（IRQ），表示请求的中断服务例程将在该中断上运行。                                                              |
		| `handler` | 中断处理函数，是中断的顶半部分的处理函数。它会在中断被触发时执行，通常用于快速的中断处理。                        |
		| `thread_fn` | 中断线程函数，是中断的底半部分的处理函数。它会在中断处理函数完成后在一个内核线程上执行，通常用于较长时间的处理。  |
		| `flags`   | 用于指定中断请求的标志和选项，例如 `IRQF_SHARED`、`IRQF_TRIGGER_*` 等。                                                |
		| `name`    | 用于标识中断请求的名称，通常是一个描述性的字符串。                                                                          |
		| `dev`     | 与中断请求相关联的设备或数据结构的指针，通常会传递设备的指针，以便中断处理函数能够访问设备特定的数据。           |

		**返回值**：成功时返回0，表示中断请求成功；失败时返回一个负数，表示中断请求失败。

		**函数说明**：`request_threaded_irq` 函数用于请求注册中断服务例程，以便在触发中断时执行相关的处理。
		通常，中断处理分为两部分：顶半部分和底半部分。顶半部分是中断处理函数（`handler`），它会在中断被触发
		时立即执行，通常执行一些快速的、紧急的操作。底半部分是中断线程函数（`thread_fn`），它会在顶半部分完
		成后异步执行，通常用于较长时间的处理、延迟处理或处理需要在不同上下文中执行的操作。
		`request_threaded_irq` 用于注册中断，指定了中断请求号 (`irq`)、中断处理函数 (`handler`)、中断线程函数 
		(`thread_fn`) 以及其他相关参数。这个函数是线程安全的，通常用于多核处理器上的中断处理。
		如果中断请求成功注册，它将返回0，否则返回一个负数，表示中断请求失败。成功注册后，中断服务例程将在中断被
		触发时按顺序执行顶半部分和底半部分。中断处理函数和线程函数应该根据实际需求执行相关的操作，
		比如读取数据、清除中断标志、通知设备状态等。
		*/
		/*== 2、申请注册中断 ==*/
		err = request_irq(gpio_keys_100ask[i].irq, gpio_key_isr, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "100ask_gpio_key", &gpio_keys_100ask[i]);
	}

	major = register_chrdev(0, "100ask_gpio_key", &gpio_key_drv);  /* /dev/100ask_gpio_key */

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

static struct platform_driver gpio_keys_driver = {
    .probe      = gpio_key_probe,
    .remove     = gpio_key_remove,
    .driver     = {
        .name   = "100ask_gpio_key",
        .of_match_table = ask100_keys,
    },
};

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



module_init(gpio_key_init);
module_exit(gpio_key_exit);

MODULE_LICENSE("GPL");


