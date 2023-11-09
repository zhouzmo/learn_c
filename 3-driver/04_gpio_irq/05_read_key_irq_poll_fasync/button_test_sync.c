
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <signal.h>

static int fd;
static void sig_func(int sig)
{
	int val;
	read(fd, &val, 4);
	printf("get button : 0x%x\n", val);
}


int main(int argc, char **argv)
{
	int val;
	struct pollfd fds[1];
	int timeout_ms = 5000;
	int ret;
	int	flags;
	
	/* 1. 判断参数 */
	if (argc != 2) 
	{
		printf("Usage: %s <dev>\n", argv[0]);
		return -1;
	}

	signal(SIGIO, sig_func);

	/* 2. 打开文件 */
	fd = open(argv[1], O_RDWR);
	if (fd == -1)
	{
		printf("can not open file %s\n", argv[1]);
		return -1;
	}

	fcntl(fd, F_SETOWN, getpid());// 将 pid 写入 file 结构体中
	flags = fcntl(fd, F_GETFL);		// 获取 file 的 flag，例如：O_RDWR|。。。
	fcntl(fd, F_SETFL, flags | FASYNC); // 给 file 的flag 加上 FASYNC

	while (1)
	{
		printf("www.100ask.net \n");
		sleep(2);
	}
	
	close(fd);
	
	return 0;
}


/*
$  echo "7417" > /proc/sys/kernel/printk

$ insmod gpio_key.ko 
[  131.555242] /home/book/learn_c/3-driver/04_gpio_irq/05_read_key_irq_poll_fasync/gpio_key_drv.c gpio_key_init line 236
[  131.581217] /home/book/learn_c/3-driver/04_gpio_irq/05_read_key_irq_poll_fasync/gpio_key_drv.c gpio_key_probe line 151
# 按下0 松开 1
[  138.974844] key 110 0
[  139.185642] key 110 1
[  141.640098] key 110 0
[  142.380689] key 110 1
[  143.143353] key 110 0
[  143.703039] key 110 1
[  145.262311] key 129 0
[  145.868634] key 129 1


$ ./button_sync_test /dev/100ask_gpio_key 
www.100ask.net
www.100ask.net
www.100ask.net
www.100ask.net
www.100ask.net
www.100ask.net
www.100ask.net
www.100ask.net
www.100ask.net
[  445.656654] key 110 0
get button : 0x6e00
www.100ask.net
[  445.883530] key 110 1
get button : 0x6e01
www.100ask.net
[  447.629559] key 110 0
get button : 0x6e00
www.100ask.net
[  447.844863] key 110 1
get button : 0x6e01
www.100ask.net
[  449.435948] key 110 0
get button : 0x6e00
www.100ask.net
[  449.654082] key 110 1
get button : 0x6e01
www.100ask.net
www.100ask.net



*/