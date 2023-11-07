
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <asm/io.h>

static int major;
static struct class *led_class;

/* registers */
// IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 地址：0x02290000 + 0x14
static volatile unsigned int *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3;

// GPIO5_GDIR 地址：0x020AC004
static volatile unsigned int *GPIO5_GDIR;

// GPIO5_DR 地址：0x020AC000
static volatile unsigned int *GPIO5_DR;

static ssize_t led_write(struct file *filp, const char __user *buf,
						 size_t count, loff_t *ppos)
{
	char val;
	int ret;

	/* 拷贝用户空间数据到内核空间 */
	ret = copy_from_user(&val, buf, 1);

	/* 根据用户指定数据，觉得 on/off LED */
	if (val)
	{
		/* led on 输出低电平*/
		*GPIO5_DR &= ~(1 << 3);
	}
	else
	{
		/*  led off 输出高电平*/
		*GPIO5_DR |= (1 << 3);
	}
	return 1;
}

static int led_open(struct inode *inode, struct file *filp)
{
	/*
	在 open 函数中配置引脚，只有应用程序使用到对应设备时，才设置对应引脚
	1. 使能GPIO：GPIO5 默认使能
	2. 引脚模式选择：将 GPIO5_3  设置为 GPIO模式
		IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3地址：0x02290000 + 0x14  bit0~3 = 101
	3. 设置引脚方向：
		GPIO5_GDIR地址：0x020AC004
		0输入/1输出，GPIO5_3，3号引脚，将该寄存器中的第 3 位改为 1
	4. 设置数据：在 write 函数里修改数据
		GPIO5_DR地址：0x020AC000
	 */
	// 清除后 4 位	~0xf = 0x1111 1111 1111 0000
	*IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 = *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 & (~0xf);
	// 清除后 4 位	0x5 = 0x0000 0000 0000 0101
	*IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 = *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 | 0x5;

	// 1 << 3 = 0000 0000 0000 0100
	*GPIO5_GDIR = *GPIO5_GDIR | (1 << 3);

	return 0;
}

static struct file_operations led_fops = {
	.owner = THIS_MODULE,
	.write = led_write,
	.open = led_open,
};

/* 入口函数 */
static int __init led_init(void)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

	/* 映射物理地址 ioremap ，可以得到 GPIO5_DR 虚拟地址，
	操作虚拟地址数据就实现物理地址数据改变*/
	// IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 地址：0x02290000 + 0x14
	IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 = ioremap(0x02290000 + 0x14, 4);

	// GPIO5_GDIR 地址：0x020AC004
	GPIO5_GDIR = ioremap(0x020AC004, 4);

	// GPIO5_DR 地址：0x020AC000
	GPIO5_DR = ioremap(0x020AC000, 4);

	major = register_chrdev(0, "100ask_led", &led_fops);
	led_class = class_create(THIS_MODULE, "myled");
	device_create(led_class, NULL, MKDEV(major, 0), NULL, "myled"); /* /dev/myled */

	return 0;
}

static void __exit led_exit(void)
{
	iounmap(IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3);
	iounmap(GPIO5_GDIR);
	iounmap(GPIO5_DR);

	device_destroy(led_class, MKDEV(major, 0));
	class_destroy(led_class);
	unregister_chrdev(major, "100ask_led");
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
