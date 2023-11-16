
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

static struct input_dev *g_input_dev;
static int g_irq;

/*== 3、在中断处理函数中，读取数据，上报数据 ==*/
static irqreturn_t input_dev_demo_isr(int irq, void *dev_id)
{
	// 读数据（todo)

	// 上报数据
	// input_event(g_input_dev, EV_KEY, XX, 0);
	// input_sync(g_input_dev); // input_event(dev, EV_SYN, SYN_REPORT, 0);

	printk("%s %s %d \n", __FILE__, __FUNCTION__, __LINE__);

	return IRQ_HANDLED;
}

static int input_dev_demo_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	int error;
	// struct resource *irq;
	int gpio;


	/*== 2、在probe  函数里，获取设备树硬件信息，构造 input_dev   结构体，注册中断函数==*/
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

	gpio = of_get_gpio(pdev->dev.of_node, 0);

	g_input_dev = devm_input_allocate_device(dev); // 分配

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

	// 支持事件 触摸屏事件，xy方向的绝对位移事件
	__set_bit(BTN_TOUCH, g_input_dev->keybit);
	__set_bit(ABS_MT_SLOT, g_input_dev->absbit);
	__set_bit(ABS_MT_POSITION_X, g_input_dev->absbit);
	__set_bit(ABS_MT_POSITION_Y, g_input_dev->absbit);

	// 设置事件上报值得最小值最大值
	input_set_abs_params(g_input_dev, ABS_MT_POSITION_X, 0, 0xffff, 0, 0);
	input_set_abs_params(g_input_dev, ABS_MT_POSITION_Y, 0, 0xffff, 0, 0);

	// 注册设备
	error = input_register_device(g_input_dev);

	// 获取设备树硬件信息，中断资源
	// irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	// g_irq = irq->start;
	g_irq = gpio_to_irq(gpio);
	//  注册中断函数
	error = request_irq(g_irq, input_dev_demo_isr, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "input_dev_demo_irq", NULL);

	return 0;
}

static int input_dev_demo_remove(struct platform_device *pdev)
{
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
$ make mrproper
$ make 100ask_imx6ull_qemu_defconfig
$ make zImage -j4

$ vim arch/arm/boot/dts/100ask_imx6ull_qemu.dts
$ make dtbs
$ cp arch/arm/boot/zImage ~/ubuntu-18.04_imx6ul_qemu_system/imx6ull-system-image/
$ cp arch/arm/boot/dts/100ask_imx6ull_qemu.dtb  ~/ubuntu-18.04_imx6ul_qemu_system/imx6ull-system-image/


$ cd ~/ubuntu-18.04_imx6ul_qemu_system
$ ./qemu-imx6ull-gui.sh


$ mount -t nfs -o nolock,vers=3 10.0.2.2:/home/book/nfs_rootfs /mnt
$ echo "7 4 1 7" > /proc/sys/kernel/printk


# insmod touchscreen_qemu.ko
[  305.197155] touchscreen_qemu: loading out-of-tree module taints kernel.
[  305.233866] /home/book/learn_c/6-driver_plus/11_input/04-touchscreen_qume/01_irq_ok/touchscreen_qemu.c input_dev_demo_init 131
[  305.288135] /home/book/learn_c/6-driver_plus/11_input/04-touchscreen_qume/01_irq_ok/touchscreen_qemu.c input_dev_demo_probe 51
[  305.358137] input: input_dev_demo as /devices/soc0/21b4000.input_dev_demo/input/input3
[  315.662577] /home/book/learn_c/6-driver_plus/11_input/04-touchscreen_qume/01_irq_ok/touchscreen_qemu.c input_dev_demo_isr 37
[  315.862710] /home/book/learn_c/6-driver_plus/11_input/04-touchscreen_qume/01_irq_ok/touchscreen_qemu.c input_dev_demo_isr 37
[  329.646219] /home/book/learn_c/6-driver_plus/11_input/04-touchscreen_qume/01_irq_ok/touchscreen_qemu.c input_dev_demo_isr 37
[  330.003619] /home/book/learn_c/6-driver_plus/11_input/04-touchscreen_qume/01_irq_ok/touchscreen_qemu.c input_dev_demo_isr 37
[  333.823798] /home/book/learn_c/6-driver_plus/11_input/04-touchscreen_qume/01_irq_ok/touchscreen_qemu.c input_dev_demo_isr 37
[  334.438371] /home/book/learn_c/6-driver_plus/11_input/04-touchscreen_qume/01_irq_ok/touchscreen_qemu.c input_dev_demo_isr 37




*/