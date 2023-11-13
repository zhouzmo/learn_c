
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

/*
 * ./ledtest /dev/100ask_led0 on
 * ./ledtest /dev/100ask_led0 off
 */
int main(int argc, char **argv)
{
	int fd;
	char status;
	
	/* 1. 判断参数 */
	if (argc != 3) 
	{
		printf("Usage: %s <dev> <on | off>\n", argv[0]);
		return -1;
	}

	/* 2. 打开文件 */
	fd = open(argv[1], O_RDWR);
	if (fd == -1)
	{
		printf("can not open file %s\n", argv[1]);
		return -1;
	}

	/* 3. 写文件 */
	if (0 == strcmp(argv[2], "on"))
	{
		status = 1;
		write(fd, &status, 1);
	}
	else
	{
		status = 0;
		write(fd, &status, 1);
	}
	
	close(fd);
	
	return 0;
}



/*
$ ls *.ko
board_A_led.ko  chip_demo_gpio.ko  leddrv.ko
$ insmod leddrv.ko
$ insmod board_A_led.ko
$ insmod chip_demo_gpio.ko
$ ls /dev/100ask_led*
/dev/100ask_led0  /dev/100ask_led1


$ ./ledtest /dev/100ask_led0 on
$ echo "7417" > /proc/sys/kernel/printk
$ 

[  239.396206] /home/book/learn_c/3-driver/02_driver_module_gpio/05_led_drv_bus_dev_drv_template/leddrv.c led_drv_open line 77
[  239.407752] init gpio: group 3, pin 1
[  239.411445] init pin of group 3 ...
[  239.416436] /home/book/learn_c/3-driver/02_driver_module_gpio/05_led_drv_bus_dev_drv_template/leddrv.c led_drv_write line 64
[  239.428456] set led on: group 3, pin 1
[  239.432238] set pin of group 3 ...
[  239.436547] /home/book/learn_c/3-driver/02_driver_module_gpio/05_led_drv_bus_dev_drv_template/leddrv.c led_drv_close line 86

$ ./ledtest /dev/100ask_led0 off
[  261.892157] /home/book/learn_c/3-driver/02_driver_module_gpio/05_led_drv_bus_dev_drv_template/leddrv.c led_drv_open line 77
[  261.906816] init gpio: group 3, pin 1
[  261.910550] init pin of group 3 ...
[  261.916430] /home/book/learn_c/3-driver/02_driver_module_gpio/05_led_drv_bus_dev_drv_template/leddrv.c led_drv_write line 64
[  261.929182] set led off: group 3, pin 1
[  261.933086] set pin of group 3 ...
[  261.938046] /home/book/learn_c/3-driver/02_driver_module_gpio/05_led_drv_bus_dev_drv_template/leddrv.c led_drv_close line 86
$





*/


