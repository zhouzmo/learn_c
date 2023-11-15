
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

/*
$ insmod 100ask_led.ko
$ ./ledtest /dev/100ask_led0 on
$ dmesg |tail -n 20

[  255.136224] /home/book/shareFile/ubuntuShareFIle/learn_c/3-driver/04_GPIO/02_led_drv_template/leddrv.c led_drv_open line 58
[  255.136275] /home/book/shareFile/ubuntuShareFIle/learn_c/3-driver/04_GPIO/02_led_drv_template/board_demo.c board_demo_led_init line 22, led 0
[  255.136368] /home/book/shareFile/ubuntuShareFIle/learn_c/3-driver/04_GPIO/02_led_drv_template/leddrv.c led_drv_write line 45
[  255.136403] /home/book/shareFile/ubuntuShareFIle/learn_c/3-driver/04_GPIO/02_led_drv_template/board_demo.c board_demo_led_ctl line 28, led 0, on
[  255.136466] /home/book/shareFile/ubuntuShareFIle/learn_c/3-driver/04_GPIO/02_led_drv_template/leddrv.c led_drv_close line 67
$

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


