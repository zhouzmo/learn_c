
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>

static char *ev_names[] = {
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
	
	if (argc != 2)
	{
		printf("Usage: %s <dev>\n", argv[0]);
		return -1;
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0)
	{
		printf("open %s err\n", argv[1]);
		return -1;
	}

/*
F:\SIProject\Linux-4.9.88\drivers\input\evdev.c 驱动代码
static long evdev_do_ioctl(struct file *file, unsigned int cmd,
			   void __user *p, int compat_mode)
{
	struct evdev_client *client = file->private_data;
	struct evdev *evdev = client->evdev;
	struct input_dev *dev = evdev->handle.dev;
	struct input_absinfo abs;
	struct input_mask mask;
	struct ff_effect effect;
	int __user *ip = (int __user *)p;
	unsigned int i, t, u, v;
	unsigned int size;
	int error;


	switch (cmd) {

	case EVIOCGVERSION:
		return put_user(EV_VERSION, ip);

	case EVIOCGID:
		if (copy_to_user(p, &dev->id, sizeof(struct input_id)))
			return -EFAULT;
		return 0;

	case EVIOCGREP:

*/
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

	return 0;
}
/*
触摸屏 N: Name="goodix-ts"
$ ./input_info.out /dev/input/event1
bustype = 0x18
vendor  = 0xdead
product = 0xbeef
version = 0x28bb
support ev type: EV_SYN  EV_KEY  EV_ABS

按鍵 N: Name="gpio-keys"
$ ./input_info.out /dev/input/event2
bustype = 0x19
vendor  = 0x1
product = 0x1
version = 0x100
support ev type: EV_SYN  EV_KEY

*/