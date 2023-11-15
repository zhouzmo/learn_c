
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>

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

int main(int argc, char **argv)
{
	int fd;
	int ret;
	struct pollfd fds[1];
	struct input_event event;
	nfds_t nfds = 1;
	short revents_temp;

	if (argc != 2)
	{
		printf("Usage: %s <dev>\n", argv[0]);
	}

	fd = open(argv[1], O_RDWR | O_NONBLOCK);
	if (fd < 0)
	{
		printf("open %s fail", argv[0]);
	}

	fds[0].fd = fd;
	fds[0].events = POLLIN;
	// fds[0].events = POLLOUT;
	fds[0].revents = 0;
/*
static unsigned int evdev_poll(struct file *file, poll_table *wait)
{
	struct evdev_client *client = file->private_data;
	struct evdev *evdev = client->evdev;
	unsigned int mask;

	poll_wait(file, &evdev->wait, wait);

	if (evdev->exist && !client->revoked)
		mask = POLLOUT | POLLWRNORM;
	else
		mask = POLLHUP | POLLERR;

	if (client->packet_head != client->tail)
		mask |= POLLIN | POLLRDNORM;

	return mask;
}
*/


	while (1)
	{
		/*
		int poll(struct pollfd fds[], nfds_t nfds, int timeout);
		fds：待查询fd，需要查询的事件，事件的返回值
		nfds:待查询文件数量
		返回值：-1 错误 0 超时（规定事件未发生事件） ，发生事件数量
		*/

		ret = poll(fds, nfds, 3000);// 阻塞等待 3s

		// 如果有事件发生，且发生事件为 POLLIN
		if (ret > 0)
		{
			revents_temp = fds[0].revents;
			printf("succes poll() ret = %d,fds[0].revents = %d\n", ret, revents_temp);

			if (fds[0].revents == POLLIN)
			{
				while (read(fd, &event, sizeof(event)) == sizeof(event))
				{
					printf("read event : type = 0x%x,code = 0x%x，value = 0x%x\n", event.type, event.code, event.value);
				}
			}
		}
		if (ret <= 0)
		{
			printf("error/timeout poll() ret = %d\n", ret);
		}
	}
}

/*
./04_input_read_poll.out  /dev/input/event2

error/timeout poll() ret = 0
error/timeout poll() ret = 0
error/timeout poll() ret = 0

succes poll() ret = 1,fds[0].revents = 1
read event : type = 0x1,code = 0x3，value = 0x1
read event : type = 0x0,code = 0x0，value = 0x0

succes poll() ret = 1,fds[0].revents = 1
read event : type = 0x1,code = 0x3，value = 0x0
read event : type = 0x0,code = 0x0，value = 0x0
error/timeout poll() ret = 0
error/timeout poll() ret = 0
error/timeout poll() ret = 0
error/timeout poll() ret = 0

*/

/*

#define POLLIN		0x0001
#define POLLPRI		0x0002
#define POLLOUT		0x0004
#define POLLERR		0x0008
#define POLLHUP		0x0010
#define POLLNVAL	0x0020

#define POLLRDNORM	0x0040
#define POLLRDBAND	0x0080
#ifndef POLLWRNORM
#define POLLWRNORM	0x0100
#endif
#ifndef POLLWRBAND
#define POLLWRBAND	0x0200
#endif
#ifndef POLLMSG
#define POLLMSG		0x0400
#endif
#ifndef POLLREMOVE
#define POLLREMOVE	0x1000
#endif
#ifndef POLLRDHUP
#define POLLRDHUP       0x2000
#endif

#define POLLFREE	0x4000	

#define POLL_BUSY_LOOP	0x8000

struct pollfd {
	int fd;
	short events;
	short revents;
};
*/