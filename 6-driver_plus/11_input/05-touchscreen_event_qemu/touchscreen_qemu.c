
#include <linux/module.h>

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/gpio_keys.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_gpio.h>
#include <linux/spinlock.h>

#define TOUCHSCREEN_POLL_TIME_MS 10

// ioremap 到内存，在 qume 中，这 4 个值正好对应
struct qemu_ts_con
{
	volatile unsigned int pressure; // 记录触摸屏压力值，只有0、1两个取值，1表示被按下，0表示松开
	volatile unsigned int x;
	volatile unsigned int y;
	volatile unsigned int clean; // 写入任意值，就会清零上述3个寄存器(仅用于测试，不用也可)
};

static struct input_dev *g_input_dev;
static int g_irq;
static struct qemu_ts_con *ts_con;
struct timer_list ts_timer;

/*== 4、在定时器处理函数中，检查是否还处于按压，是，说明有滑动事件，上报 ==*/
static void ts_irq_timer(unsigned long _data)
{
	// 还是按压状态，修改超时时间，上报事件
	if (ts_con->pressure) 
	{
		input_event(g_input_dev, EV_ABS, ABS_X, ts_con->x);
		input_event(g_input_dev, EV_ABS, ABS_Y, ts_con->y);
		input_sync(g_input_dev);

		mod_timer(&ts_timer,
				  jiffies + msecs_to_jiffies(TOUCHSCREEN_POLL_TIME_MS));
	}
}

/*== 3、在中断处理函数中，读取数据，上报数据 ==*/
static irqreturn_t input_dev_demo_isr(int irq, void *dev_id)
{

	// printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	if (ts_con->pressure)
	{
		// 按下上报数据
		input_event(g_input_dev, EV_ABS, ABS_X, ts_con->x);
		input_event(g_input_dev, EV_ABS, ABS_Y, ts_con->y);
		// 按键类事件 ，1表示按下
		input_event(g_input_dev, EV_KEY, BTN_TOUCH, 1);
		input_sync(g_input_dev);

		// 修改定时器超时时间，10ms 超时，10ms再次检查是否按下(寄存器是否改变)，按下继续上报当前数据
		mod_timer(&ts_timer,
				  jiffies + msecs_to_jiffies(TOUCHSCREEN_POLL_TIME_MS));
	}
	else
	{
		// 松开上报数据
		input_event(g_input_dev, EV_KEY, BTN_TOUCH, 0);
		input_sync(g_input_dev);

		
	}

	return IRQ_HANDLED;
}

static int input_dev_demo_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	int error;
	// struct resource *irq;
	struct resource *io;
	int gpio;

	/*== 2、在probe  函数里，获取设备树硬件信息，构造 input_dev   结构体，注册中断函数==*/
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

	gpio = of_get_gpio(pdev->dev.of_node, 0);


	// 分配
	g_input_dev = devm_input_allocate_device(dev); 

	// 设置 input_dev
	g_input_dev->name = "input_dev_demo";
	g_input_dev->phys = "input_dev_demo";
	g_input_dev->dev.parent = dev;
	// 设置 input_id
	g_input_dev->id.bustype = BUS_HOST;
	g_input_dev->id.vendor = 0x0001;
	g_input_dev->id.product = 0x0001;
	g_input_dev->id.version = 0x0100;

	// 支持事件类型 按键类型，绝对位移事件类型
	__set_bit(EV_KEY, g_input_dev->evbit);
	__set_bit(EV_ABS, g_input_dev->evbit);
	__set_bit(INPUT_PROP_DIRECT, g_input_dev->propbit);

	// 支持事件 触摸屏事件，xy方向的绝对位移事件
	__set_bit(BTN_TOUCH, g_input_dev->keybit);
	__set_bit(ABS_X, g_input_dev->absbit);
	__set_bit(ABS_Y, g_input_dev->absbit);
	// 设置该属性位，tslib 会找到该设备
	__set_bit(INPUT_PROP_DIRECT, g_input_dev->propbit);

	// 设置事件上报值得最小值最大值
	input_set_abs_params(g_input_dev, ABS_X, 0, 0xffff, 0, 0);
	input_set_abs_params(g_input_dev, ABS_Y, 0, 0xffff, 0, 0);

	// 注册设备
	error = input_register_device(g_input_dev);

	// 获取设备树硬件信息，中断资源
	io = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	ts_con = ioremap(io->start, io->end - io->start + 1);

	// irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	// g_irq = irq->start;
	g_irq = gpio_to_irq(gpio);
	//  注册中断函数
	error = request_irq(g_irq, input_dev_demo_isr, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "input_dev_demo_irq", NULL);

	setup_timer(&ts_timer,
				ts_irq_timer, (unsigned long)NULL);

	return 0;
}

static int input_dev_demo_remove(struct platform_device *pdev)
{
	del_timer_sync(&ts_timer);
	iounmap(ts_con);
	free_irq(g_irq, NULL);
	input_unregister_device(g_input_dev);
	return 0;
}

/* 设备树文件
/ {
	input_dev_demo {
		compatible = "100ask,input_dev_demo";
		reg = <0x021B4000 16>;
		//interrupt-parent = <&gpio1>;
		//interrupts = <5 IRQ_TYPE_EDGE_FALLING | IRQ_TYPE_EDGE_RISING>;
		gpios = <&gpio1 5 1>;
	};
};

*/
/*== 1、构建    platform_driver，对应设备树构建 platform_device，对应设备树构建 ==*/
static const struct of_device_id input_dev_demo_of_match[] = {
	{
		.compatible = "100ask,input_dev_demo",
	},
	{},
};

static struct platform_driver input_dev_demo_driver = {
	.probe = input_dev_demo_probe,
	.remove = input_dev_demo_remove,
	.driver = {
		.name = "input_dev_demo",
		.of_match_table = input_dev_demo_of_match,
	}};

static int __init input_dev_demo_init(void)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	return platform_driver_register(&input_dev_demo_driver);
}

static void __exit input_dev_demo_exit(void)
{
	platform_driver_unregister(&input_dev_demo_driver);
}

module_init(input_dev_demo_init);
module_exit(input_dev_demo_exit);

MODULE_LICENSE("GPL");


/*
$ ls /dev/input/*
$  insmod touchscreen_qemu.ko
$ ls /dev/input/*


$ hexdump /dev/input/event3
按下
0000000 18eb 0000 21e1 0004 0003 0000 21ed 0000
0000010 18eb 0000 21e1 0004 0003 0001 3d24 0000
0000020 18eb 0000 21e1 0004 0001 014a 0001 0000
0000030 18eb 0000 21e1 0004 0000 0000 0000 0000
松开
0000040 18f0 0000 ac54 000d 0001 014a 0000 0000
0000050 18f0 0000 ac54 000d 0000 0000 0000 0000

按下
0000000 19d7 0000 eb85 0000 0003 0000 3366 0000
0000010 19d7 0000 eb85 0000 0003 0001 3ef4 0000
0000020 19d7 0000 eb85 0000 0001 014a 0001 0000
0000030 19d7 0000 eb85 0000 0000 0000 0000 0000
滑动
0000040 19e7 0000 d081 000b 0003 0001 3f5f 0000
0000050 19e7 0000 d081 000b 0000 0000 0000 0000
0000060 19e8 0000 7eab 0000 0003 0000 3332 0000
0000070 19e8 0000 7eab 0000 0003 0001 3fa6 0000
0000080 19e8 0000 7eab 0000 0000 0000 0000 0000
0000090 19e8 0000 d575 0001 0003 0001 3fed 0000
00000a0 19e8 0000 d575 0001 0000 0000 0000 0000
00000b0 19e8 0000 f6de 0002 0003 0001 4011 0000
00000c0 19e8 0000 f6de 0002 0000 0000 0000 0000
00000d0 19e8 0000 311b 0004 0003 0000 32fe 0000
00000e0 19e8 0000 311b 0004 0003 0001 4058 0000
00000f0 19e8 0000 311b 0004 0000 0000 0000 0000
0000100 19e8 0000 1feb 0005 0003 0001 409f 0000
0000110 19e8 0000 1feb 0005 0000 0000 0000 0000
松开
0000120 19f1 0000 4a71 000a 0001 014a 0000 0000
0000130 19f1 0000 4a71 000a 0000 0000 0000 0000




*/