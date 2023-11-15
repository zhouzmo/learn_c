
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

char *ev_names[] = {
	"EV_SYN ",
	"EV_KEY ",
	"EV_REL ",
	"EV_ABS ",
	"EV_MSC ",
	"EV_SW	",
	"NULL ",
	"NULL ",
	"NULL ",
	"NULL ",
	"NULL ",
	"NULL ",
	"NULL ",
	"NULL ",
	"NULL ",
	"NULL ",
	"NULL ",
	"EV_LED ",
	"EV_SND ",
	"NULL ",
	"EV_REP ",
	"EV_FF	",
	"EV_PWR ",
};

int fd;

void my_sig_handler(int sig)
{
	struct input_event event;
	while (read(fd, &event, sizeof(event)) == sizeof(event))
	{
		printf("get event: type = 0x%x, code = 0x%x, value = 0x%x\n", event.type, event.code, event.value);
	}
}

int main(int argc, char **argv)
{
	int err;
	int len;
	int ret;
	int i;
	unsigned char byte;
	int bit;
	struct input_id id;
	unsigned int evbit[2];
	unsigned int flags;
	int count = 0;

	if (argc != 2)
	{
		printf("Usage: %s <dev>\n", argv[0]);
		return -1;
	}

	/* 注册信号处理函数 */
	signal(SIGIO, my_sig_handler);

	/* 打开驱动程序 */
	fd = open(argv[1], O_RDWR | O_NONBLOCK);
	if (fd < 0)
	{
		printf("open %s err\n", argv[1]);
		return -1;
	}

	/* 把APP的进程号告诉驱动程序 */
	fcntl(fd, F_SETOWN, getpid());

	/* 使能"异步通知" */
	flags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, flags | FASYNC);

	while (1)
	{
		printf("main loop count = %d\n", count++);
		sleep(2);
	}

	return 0;
}
