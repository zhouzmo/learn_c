
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <signal.h>

static int fd;

/*
 * ./button_test /dev/100ask_button0
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
	fd = open(argv[1], O_RDWR | O_NONBLOCK);
	if (fd == -1)
	{
		printf("can not open file %s\n", argv[1]);
		return -1;
	}

	for (i = 0; i < 10; i++) 
	{
		if (read(fd, &val, 4) == 4)
			printf("get button: 0x%x\n", val);
		else
			printf("get button: -1\n");
	}

	flags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);

	while (1)
	{
		if (read(fd, &val, 4) == 4)
			printf("get button: 0x%x\n", val);
		else
			printf("while get button: -1\n");
	}
	
	close(fd);
	
	return 0;
}


/*
$ insmod gpio_key_drv.ko 

[ 1011.014278] /home/book/learn_c/3-driver/04_gpio_irq/09_block_timer_tasklet/gpio_key_drv.c gpio_key_init line 271
[ 1011.035530] /home/book/learn_c/3-driver/04_gpio_irq/09_block_timer_tasklet/gpio_key_drv.c gpio_key_probe line 178
[ 1011.054440] key_timer_expire key 110 1
[ 1011.058241] key_timer_expire key 129 1
$ ./button_test /dev/100ask_gpio_key
get button: 0x6e01
get button: 0x8101
get button: -1
get button: -1
get button: -1
get button: -1
get button: -1
get button: -1
get button: -1
get button: -1
[ 1022.924947] key_tasklet_func key 110 0
[ 1022.954417] key_timer_expire key 110 0
get button: 0x6e00
[ 1023.131004] key_tasklet_func key 110 1
[ 1023.154588] key_timer_expire key 110 1
get button: 0x6e01
[ 1024.237889] key_tasklet_func key 110 0
[ 1024.264466] key_timer_expire key 110 0
get button: 0x6e00
[ 1024.493994] key_tasklet_func key 110 1
[ 1024.514434] key_timer_expire key 110 1
get button: 0x6e01
[ 1025.241946] key_tasklet_func key 110 0
[ 1025.264427] key_timer_expire key 110 0
get button: 0x6e00
[ 1025.477057] key_tasklet_func key 110 1
[ 1025.504365] key_timer_expire key 110 1
get button: 0x6e01
[ 1026.126915] key_tasklet_func key 110 0
[ 1026.154438] key_timer_expire key 110 0
get button: 0x6e00
[ 1027.020591] key_tasklet_func key 110 1
[ 1027.044425] key_timer_expire key 110 1
get button: 0x6e01
[ 1030.030081] key_tasklet_func key 129 0
[ 1030.054469] key_timer_expire key 129 0
get button: 0x8100
[ 1030.261987] key_tasklet_func key 129 1
[ 1030.284439] key_timer_expire key 129 1
get button: 0x8101
[ 1030.851004] key_tasklet_func key 129 0
[ 1030.874455] key_timer_expire key 129 0
get button: 0x8100
[ 1031.097589] key_tasklet_func key 129 1
[ 1031.124426] key_timer_expire key 129 1
get button: 0x8101
[ 1031.887572] key_tasklet_func key 129 0
[ 1031.914415] key_timer_expire key 129 0
get button: 0x8100
[ 1032.290525] key_tasklet_func key 129 1
[ 1032.314426] key_timer_expire key 129 1
get button: 0x8101

*/