
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

[ 4023.972674] /home/book/learn_c/3-driver/04_GPIO/04_led_drv_template_seperate/leddrv.c led_drv_open line 58
[ 4023.972703] init gpio: group 3, pin 1
[ 4023.972716] init pin of group 3 ...
[ 4023.972775] /home/book/learn_c/3-driver/04_GPIO/04_led_drv_template_seperate/leddrv.c led_drv_write line 45
[ 4023.972794] set led on: group 3, pin 1
[ 4023.972806] set pin of group 3 ...
[ 4023.972846] /home/book/learn_c/3-driver/04_GPIO/04_led_drv_template_seperate/leddrv.c led_drv_close line 67

*/

