
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

/* 
使能"异步通知" 
当文件状态发生变化时，内核将通知已注册的进程，而无需进程轮询文件状态。
这对于需要实时响应文件状态变化的应用程序非常有用。

fasync_helper 加入等待队列，kill_fasync 唤醒

F:\SIProject\Linux-4.9.88\drivers\input\evdev.c

static int evdev_fasync(int fd, struct file *file, int on)
{
	struct evdev_client *client = file->private_data;
	return fasync_helper(fd, file, on, &client->fasync);
}


static void __pass_event(struct evdev_client *client,
			 const struct input_event *event)
{
	client->buffer[client->head++] = *event;
	client->head &= client->bufsize - 1;

	if (unlikely(client->head == client->tail)) {
		
		client->tail = (client->head - 2) & (client->bufsize - 1);

		client->buffer[client->tail].time = event->time;
		client->buffer[client->tail].type = EV_SYN;
		client->buffer[client->tail].code = SYN_DROPPED;
		client->buffer[client->tail].value = 0;

		client->packet_head = client->tail;
	}

	if (event->type == EV_SYN && event->code == SYN_REPORT) {
		client->packet_head = client->head;
		kill_fasync(&client->fasync, SIGIO, POLL_IN);
	}
}	
*/
	flags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, flags | FASYNC);

	while (1)
	{
		printf("main loop count = %d\n", count++);
		sleep(2);
	}

	return 0;
}


/*
$ ./05_input_read_fasync.out /dev/input/event2
main loop count = 0
main loop count = 1
main loop count = 2
# 按下
get event: type = 0x1, code = 0x3, value = 0x1
get event: type = 0x0, code = 0x0, value = 0x0
main loop count = 3
# 松开
get event: type = 0x1, code = 0x3, value = 0x0
get event: type = 0x0, code = 0x0, value = 0x0
main loop count = 4
main loop count = 5
get event: type = 0x1, code = 0x2, value = 0x1
get event: type = 0x0, code = 0x0, value = 0x0
main loop count = 6
get event: type = 0x1, code = 0x2, value = 0x0
get event: type = 0x0, code = 0x0, value = 0x0
main loop count = 7
main loop count = 8


*/