
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

/*
将 led 的操作抽象出一个 led_opr 结构，里面包含led的初始化，off/on，操作等信息
led_drv 根据该结构体，实现驱动程序的编写



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
$ rmmod 100ask_led
$ insmod 100ask_led.ko
$ ./ledtest /dev/100ask_led0 on
$ ./ledtest /dev/100ask_led0 off
$ dmesg |tail -n 20

[ 2157.781972] /home/book/learn_c/3-driver/04_GPIO/03_led_drv_for_boards/leddrv.c led_drv_open line 57
[ 2157.782207] /home/book/learn_c/3-driver/04_GPIO/03_led_drv_for_boards/leddrv.c led_drv_write line 44
[ 2157.782282] /home/book/learn_c/3-driver/04_GPIO/03_led_drv_for_boards/leddrv.c led_drv_close line 66
[ 2163.598266] /home/book/learn_c/3-driver/04_GPIO/03_led_drv_for_boards/leddrv.c led_drv_open line 57
[ 2163.598385] /home/book/learn_c/3-driver/04_GPIO/03_led_drv_for_boards/leddrv.c led_drv_write line 44
[ 2163.598460] /home/book/learn_c/3-driver/04_GPIO/03_led_drv_for_boards/leddrv.c led_drv_close line 66


*/
