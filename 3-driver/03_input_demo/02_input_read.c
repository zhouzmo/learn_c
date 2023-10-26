
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

	err = ioctl(fd, EVIOCGID, &id);
	if (err == 0)
	{
		printf("bustype = 0x%x\n", id.bustype );
		printf("vendor	= 0x%x\n", id.vendor  );
		printf("product = 0x%x\n", id.product );
		printf("version = 0x%x\n", id.version );
	}

	len = ioctl(fd, EVIOCGBIT(0, sizeof(evbit)), &evbit);
	if (len > 0 && len <= sizeof(evbit))
	{
		printf("support ev type: ");
		for (i = 0; i < len; i++)
		{
			byte = ((unsigned char *)evbit)[i];
			for (bit = 0; bit < 8; bit++)
			{
				if (byte & (1<<bit)) {
					printf("%s ", ev_names[i*8 + bit]);
				}
			}
		}
		printf("\n");
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
	}

	return 0;
}

/*
./02_input_read.out /dt/event1
bustype = 0x18
vendor  = 0xdead
product = 0xbeef
version = 0x28bb
support ev type: EV_SYN  EV_KEY  EV_ABS
get event: type = 0x3, code = 0x39, value = 0x7
get event: type = 0x3, code = 0x35, value = 0x124
get event: type = 0x3, code = 0x36, value = 0x196
get event: type = 0x3, code = 0x30, value = 0x26
get event: type = 0x3, code = 0x3a, value = 0x26
get event: type = 0x1, code = 0x14a, value = 0x1
get event: type = 0x0, code = 0x0, value = 0x0
get event: type = 0x3, code = 0x39, value = 0xffffffff
get event: type = 0x1, code = 0x14a, value = 0x0
get event: type = 0x0, code = 0x0, value = 0x0




*/

