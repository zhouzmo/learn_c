
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <signal.h>

static int fd;


int main(int argc, char **argv)
{
	char buf[2];
	int ret;

	int i;
	
	/* 1. 判断参数 */
	if (argc != 2) 
	{
		printf("Usage: %s <dev>\n", argv[0]);
		return -1;
	}

	/* 2. 打开文件 */
	fd = open(argv[1], O_RDWR | O_NONBLOCK);
	if (fd == -1)
	{
		printf("can not open file %s\n", argv[1]);
		return -1;
	}

	while (1)
	{
		if (read(fd, buf, 2) == 2)
			printf("====================get Humidity湿度: %d, Temperature温度: %d\n", buf[0], buf[1]);
		else
			printf("get dht11: -1\n");

		sleep(5);
	}

	//sleep(30);

	close(fd);
	
	return 0;
}



/*
./button_test /dev/mydht11

key_timer_expire 111, 中断产生次数 81
parse_dht11_datas line:231, 
datas[0]=235 datas[1]=202 datas[2]=155， datas[3]=6，datas[4]=81，crc=86
parse_dht11_datas line:233, 中断产生次数 81

问题：
数据基本不能使用
发现基本的 crc 校验存在问题，中断容易丢失，数据容易错误
建议不使用中断，使用直接读取的方式

原因：
温湿度检测中断之间是 us 级别，时间过短
在检测过程中，出现类似 网络中断等等中断，就会导致温湿度检测的中断丢失


*/