
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <signal.h>
#include <sys/ioctl.h>

#define CMD_TRIG  100

static int fd;

/*
 * ./button_test /dev/sr04
 *
 */
int main(int argc, char **argv)
{
	int val;
	struct pollfd fds[1];
	int timeout_ms = 5000;
	int ret;
	int	flags;

	int i;
	
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

	fds[0].fd = fd;
	fds[0].events = POLLIN;

	while (1)
	{
		ioctl(fd, CMD_TRIG);
		printf("I am goning to read distance: \n");
		
		

		if (1 == poll(fds, 1, 5000))
		{
			if (read(fd, &val, 4) == 4)
				printf("get distance: %d cm\n", val*17/1000000);
			else
				printf("get distance err\n");
		}
		else
		{
			printf("get distance poll timeout/err\n");
		}

		sleep(1);
	}

	close(fd);
	
	return 0;
}


/*
应用程序在 read 获取上升沿和下降沿时间差，
可能由于距离过远等等原因，无法收到返回的声波

改进
1、使用 poll 监测是否产生数据，产生数据就返回
2、在 ioctl 让 tring 引脚发出信号后，开启一个定时器，让


*/