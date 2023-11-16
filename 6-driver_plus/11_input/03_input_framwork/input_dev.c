
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
#include <linux/spinlock.h>

static struct input_dev *g_input_dev;
static int g_irq;

/*== 3、在中断处理函数中，读取数据，上报数据 ==*/
static irqreturn_t input_dev_demo_isr(int irq, void *dev_id)
{
	// 读数据（todo)

	// 上报数据
	input_event(g_input_dev, EV_KEY, XX, 0);
	input_sync(g_input_dev); // input_event(dev, EV_SYN, SYN_REPORT, 0);
	
	return IRQ_HANDLED;
}



static int input_dev_demo_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	int error;
	struct resource *irq;

	/*== 2、在probe  函数里，获取设备树硬件信息，构造 input_dev   结构体，注册中断函数==*/

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
	irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	g_irq = irq->start;
	//  注册中断函数
	request_irq(irq->start, input_dev_demo_isr, IRQF_TRIGGER_RISING, "input_dev_demo_irq", NULL);

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
		interrupts = <...>;
	};
};
*/
/*== 1、构建    platform_driver，对应设备树构建 platform_device，对应设备树构建 ==*/
static const struct of_device_id input_dev_demo_of_match[] = {
	{ .compatible = "100ask,input_dev_demo", },
	{ },
};

static struct platform_driver input_dev_demo_driver = {
	.probe		= input_dev_demo_probe,
	.remove	    = input_dev_demo_remove,
	.driver		= {
		.name	= "input_dev_demo",
		.of_match_table = input_dev_demo_of_match,
	}
};


static int __init input_dev_demo_init(void)
{
	return platform_driver_register(&input_dev_demo_driver);
}

static void __exit input_dev_demo_exit(void)
{
	platform_driver_unregister(&input_dev_demo_driver);
}



module_init(input_dev_demo_init);
module_exit(input_dev_demo_exit);

MODULE_LICENSE("GPL");


