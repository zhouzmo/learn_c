
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


/* ./01_get_input_info /dev/input/event0 noblock */
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
		sleep(1);
	}

	return 0;
}

/*
[root@100ask:/mnt/learn_c/5-app/11_input/01_app_demo]# ./02_input_read.out /dev/input/event2 noblock
bustype = 0x19
vendor  = 0x1
product = 0x1
version = 0x100
support ev type: EV_SYN  EV_KEY
read err -1
read err -1
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
get event: type = 0x1, code = 0x3, value = 0x1
get event: type = 0x0, code = 0x0, value = 0x0
get event: type = 0x1, code = 0x3, value = 0x0
get event: type = 0x0, code = 0x0, value = 0x0



*/