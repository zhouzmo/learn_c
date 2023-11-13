
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
ubuntu
$ make
$ vim 100ask_imx6ull-sdk/Linux-4.9.88/arch/arm/boot/dts/100ask_imx6ull-14x14.dts
# 添加设备树内容
$ make dtbs


imx6ull
# 替换设备树
$ cp 100ask_imx6ull-sdk/Linux-4.9.88/arch/arm/boot/dts/100ask_imx6ull-14x14.dtb /boot/
$ reboot

$  cd /sys/firmware/devicetree/base/
$  ls -ld *100ask*
drwxr-xr-x 2 root root 0 Jan  1 00:28 100ask_led@0
drwxr-xr-x 2 root root 0 Jan  1 00:28 100ask_led@1
drwxr-xr-x 2 root root 0 Jan  1 00:28 gpio_keys_100ask
$  cd 100ask_led\@0/
$  ls
compatible  name  pin
$ cat name
100ask_led
$ cat compatible
100as,leddrv
$  hexdump pin
0000000 0300 0100
0000004



$ insmod leddrv.ko
$ insmod chip_demo_gpio_opr.ko
$ echo "7417" > /proc/sys/kernel/printk

$ ./ledtest /dev/100ask_led0 on
[  806.806798] /home/book/learn_c/3-driver/02_driver_module_gpio/06_led_drv_device_tree_template/leddrv.c led_drv_open line 77
[  806.819186] init gpio: group 3, pin 1
[  806.822881] init pin of group 3 ...
[  806.827858] /home/book/learn_c/3-driver/02_driver_module_gpio/06_led_drv_device_tree_template/leddrv.c led_drv_write line 64
[  806.839864] set led on: group 3, pin 1
[  806.843650] set pin of group 3 ...
[  806.847963] /home/book/learn_c/3-driver/02_driver_module_gpio/06_led_drv_device_tree_template/leddrv.c led_drv_close line 86

$ ./ledtest /dev/100ask_led0 off
[  831.559844] /home/book/learn_c/3-driver/02_driver_module_gpio/06_led_drv_device_tree_template/leddrv.c led_drv_open line 77
[  831.574502] init gpio: group 3, pin 1
[  831.578205] init pin of group 3 ...
[  831.581783] /home/book/learn_c/3-driver/02_driver_module_gpio/06_led_drv_device_tree_template/leddrv.c led_drv_write line 64
[  831.597450] set led off: group 3, pin 1
[  831.601318] set pin of group 3 ...
[  831.609778] /home/book/learn_c/3-driver/02_driver_module_gpio/06_led_drv_device_tree_template/leddrv.c led_drv_close line 86


*/