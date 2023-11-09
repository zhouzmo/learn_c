
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

/*
 * ./button_test /dev/100ask_button0
 *
 */
int main(int argc, char **argv)
{
	int fd;
	int val;
	
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

	while (1)
	{
		/* 3. 读文件 */
		read(fd, &val, 4);
		printf("get button : 0x%x\n", val);
	}
	
	close(fd);
	
	return 0;
}


/*
$ insmod gpio_key_drv.ko
# 调用 platform_driver_register，系统自动匹配，匹配成功调用 gpio_key_probe 函数
[  498.432580] /home/book/learn_c/3-driver/04_gpio_irq/02_read_key_irq/gpio_key_drv.c gpio_key_init line 218
[  498.450425] /home/book/learn_c/3-driver/04_gpio_irq/02_read_key_irq/gpio_key_drv.c gpio_key_probe line 108
$ ./button_test /dev/100ask_gpio_key &
[1] 364

# 按下为0，松开为 1
[  509.637767] key 129 0
get button : 0x8100
[  509.807230] key 129 1
get button : 0x8101
[  510.969753] key 110 0
get button : 0x6e00
[  511.138803] key 110 1
get button : 0x6e01
[  513.033124] key 129 0
get button : 0x8100
[  513.219936] key 129 1
get button : 0x8101
[  514.071177] key 110 0
get button : 0x6e00
[  514.289890] key 110 1
$ insmod gpio_key_drv.ko


$ top
 PID  PPID USER     STAT   VSZ 	%VSZ  %CPU COMMAND
 ......
  364   307 root     S     1720   0%   0% ./button_test /dev/100ask_gpio_key

不占用 cpu 资源

*/