
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
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

int main(int argc, char **argv)
{
	int fd;
	int err;
	int len;
	int i;
	unsigned char byte;
	int bit;
	struct input_id id;
	unsigned int evbit[2];
	struct input_event event;

	if (argc < 2)
	{
		printf("Usage: %s <dev> [noblock]\n", argv[0]);
		return -1;
	}

	if (argc == 3 && !strcmp(argv[2], "noblock"))
	{
		fd = open(argv[1], O_RDWR | O_NONBLOCK);
	}
	else
	{
		fd = open(argv[1], O_RDWR);
	}

	if (fd < 0)
	{
		printf("open %s err\n", argv[1]);
		return -1;
	}

	



/*
为何保存数据到 struct input_event event;，因为内核里面是用这个保存的数据
while (read + input_event_size() <= count &&
		evdev_fetch_next_event(client, &event)) {

	if (input_event_to_user(buffer + read, &event))
		return -EFAULT;

	read += input_event_size();
}


int input_event_to_user(char __user *buffer,
			const struct input_event *event)
{
	if (in_compat_syscall() && !COMPAT_USE_64BIT_TIME) {
		struct input_event_compat compat_event;

		compat_event.time.tv_sec = event->time.tv_sec;
		compat_event.time.tv_usec = event->time.tv_usec;
		compat_event.type = event->type;
		compat_event.code = event->code;
		compat_event.value = event->value;

		if (copy_to_user(buffer, &compat_event,
				 sizeof(struct input_event_compat)))
			return -EFAULT;

	} else {
		if (copy_to_user(buffer, event, sizeof(struct input_event)))
			return -EFAULT;
	}

	return 0;
}

*/

	while (1)
	{
		len = read(fd, &event, sizeof(event));
		if (len == sizeof(event))
		{
			printf("get event: type = 0x%x, code = 0x%x, value = 0x%x\n", event.type, event.code, event.value);
		}
		else
		{
			printf("read err %d\n", len);
		}
		// sleep(1);
	}

	return 0;
}


/* 
$ arm-buildroot-linux-gnueabihf-gcc 02_input_read.c -o 02_input_read.out

$ ./02_input_read.out /dev/input/event2 noblock
read err -1
read err -1
read err -1
read err -1
get event: type = 0x1, code = 0x3, value = 0x1
get event: type = 0x0, code = 0x0, value = 0x0
get event: type = 0x1, code = 0x3, value = 0x0
get event: type = 0x0, code = 0x0, value = 0x0
read err -1
read err -1
read err -1
read err -1

$ ./02_input_read.out /dev/input/event2
按下
get event: type = 0x1, code = 0x3, value = 0x1
get event: type = 0x0, code = 0x0, value = 0x0
松开
get event: type = 0x1, code = 0x3, value = 0x0
get event: type = 0x0, code = 0x0, value = 0x0





*/
