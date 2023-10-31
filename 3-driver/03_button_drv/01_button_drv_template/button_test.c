
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char **argv)
{
	int fd;
	char val;
	
	/* 1. 判断参数 */
	if (argc != 2) 
	{
		printf("Usage: %s <dev>\n", argv[0]);
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
	read(fd, &val, 1);
	printf("get button : %d\n", val);
	
	close(fd);
	
	return 0;
}


/*
make


./button_test /dev/100ask_button0

$ insmod button_drv.ko
$ insmod board_xxx.ko
$ ls /dev/100ask_button*
/dev/100ask_button0  /dev/100ask_button1
$ ./button_test /dev/100ask_button0
get button : 1
$ dmesg |tail -n 20

[15111.147620] /home/book/learn_c/3-driver/03_button_drv_todo/01_button_drv_template/board_xxx.c board_xxx_button_init_gpio 28, init gpio for button 0
[15111.147729] /home/book/learn_c/3-driver/03_button_drv_todo/01_button_drv_template/board_xxx.c board_xxx_button_read_gpio 33, read gpio for button 0
$


*/