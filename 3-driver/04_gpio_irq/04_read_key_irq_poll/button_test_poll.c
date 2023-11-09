
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>


int main(int argc, char **argv)
{
	int fd;
	int val;
	struct pollfd fds[1];
	int timeout_ms = 5000;
	int ret;
	
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
	
	// 待监听的文件句柄、要监听的事件类型
	fds[0].fd = fd;
	fds[0].events = POLLIN;
	


/*
	这是一个使用`poll`函数的代码片段，其目的是等待一组文件描述符上的事件。

	用法：`poll` 函数用于等待一组文件描述符上的事件。

	int poll(struct pollfd fds[], nfds_t nfds, int timeout);

	| 参数       | 描述                                                         |
	| ---------- | ------------------------------------------------------------ |
	| `fds`      | 一个指向 `struct pollfd` 数组的指针，其中包含要监视的文件描述符和所需的事件信息。 |
	| `nfds`     | 要监视的文件描述符的数量。                                    |
	| `timeout`  | 超时时间（以毫秒为单位）。如果设置为 `-1`，表示一直等待直到有事件发生。如果设置为 `0`，表示立即返回而不等待。其他正整数值表示等待的毫秒数。 |
	| 返回值    | - 如果成功，返回已准备好的文件描述符数量。                    
					如果超时，返回 `0`。                                       
					如果出现错误，返回 `-1` 并设置 `errno` 来指示错误。      |

	函数说明：
	- `poll` 函数用于等待一组文件描述符上的事件。
	在 `fds` 数组中设置每个文件描述符的监视事件以及相应的返回事件，
	然后通过 `timeout` 参数指定等待的超时时间。

	- `nfds` 参数表示 `fds` 数组的大小，即要监视的文件描述符的数量。

	- `timeout` 参数控制等待的时间。如果将其设置为 `-1`，则表示一直等待，
	直到有事件发生。如果设置为 `0`，则表示立即返回，不等待。
	其他正整数值表示等待的毫秒数。

	- 函数返回时，会返回已准备好的文件描述符的数量（即有事件发生的文件描述符数），
	或者如果超时则返回 `0`，或者如果出现错误则返回 `-1` 并设置 `errno` 以指示错误原因。

	在你的代码片段中，`poll` 函数会等待 `fds` 数组中前两个文件描述符上的事件，
	并将结果存储在 `num_ready` 中，`timeout_ms` 参数表示等待的毫秒数。
	如果 `timeout_ms` 为负数，表示一直等待。
*/

/*
struct pollfd {
	int fd;
	short events;
	short revents;
};
linux内部会监听 fds中的每一个 pollfd，当你调用 poll 函数时，
poll 函数会返回有对应事件发生的文件描述符数量，revents就是监听时发生的事件，会保存在 revents中

*/
	while (1)
	{
		/* 3. 首先调用drv_poll，若 5s(timeout_ms)内有事件产生，drv_poll 会被持续调用
			若 5s 内没有事件产生，最后也会调用drv_poll，返回0（超时）  */
		ret = poll(fds, 1, timeout_ms);
		// 若 fds 内部有 1 个文件描述符监听到事件，且事件类型是 POLLIN，证明文件描述符内部有事件发生
		if ((ret == 1) && (fds[0].revents & POLLIN)) 
		{
			read(fd, &val, 4);
			printf("get button : 0x%x\n", val);
		}
		else
		{
			printf("timeout\n");
		}
	}
	
	close(fd);
	
	return 0;
}




/*
修改设备树


$ echo "7417" > /proc/sys/kernel/printk
$ insmod gpio_key_drv.ko
	# 初始化设备
[ 1656.744740] gpio_key_drv.c gpio_key_init line 216
[ 1656.765601] gpio_key_drv.c gpio_key_probe line 133

# 按下 0 ，松开 1
$ ./button_test /dev/100ask_button0
[ 1914.042402] gpio_key_drv.c gpio_key_drv_poll line 94
[ 1916.851964] key 110 0
[ 1916.854578] gpio_key_drv.c gpio_key_drv_poll line 94
get button : 0x6e00
[ 1916.869624] gpio_key_drv.c gpio_key_drv_poll line 94

[ 1917.085998] key 110 1
[ 1917.088569] gpio_key_drv.c gpio_key_drv_poll line 94
get button : 0x6e01
[ 1917.101311] gpio_key_drv.c gpio_key_drv_poll line 94

[ 1918.568514] key 129 0
[ 1918.571108] gpio_key_drv.c gpio_key_drv_poll line 94
get button : 0x8100
[ 1918.583466] gpio_key_drv.c gpio_key_drv_poll line 94

[ 1918.821772] key 129 1
[ 1918.824404] gpio_key_drv.c gpio_key_drv_poll line 94
get button : 0x8101
[ 1918.839175] gpio_key_drv.c gpio_key_drv_poll line 94


[ 1923.844505] gpio_key_drv.c gpio_key_drv_poll line 94
timeout[ 1923.857196] gpio_key_drv.c gpio_key_drv_poll line 94

[ 1928.860795] gpio_key_drv.c gpio_key_drv_poll line 94
timeout[ 1928.871817] gpio_key_drv.c gpio_key_drv_poll line 94

[ 1933.875996] gpio_key_drv.c gpio_key_drv_poll line 94
timeout[ 1933.889666] gpio_key_drv.c gpio_key_drv_poll line 94

[ 1938.890957] gpio_key_drv.c gpio_key_drv_poll line 94
timeout[ 1938.906716] gpio_key_drv.c gpio_key_drv_poll line 94

[ 1943.910564] gpio_key_drv.c gpio_key_drv_poll line 94
timeout

*/



/*
# 不按下按键，timeout
$ ./button_test /dev/100ask_button0
[root@100ask:/mnt/learn_c/3-driver/04_gpio_irq/04_read_key_irq_poll]# ./button_test_poll /dev/100ask_gpio_key
[ 2305.635248] gpio_key_drv.c gpio_key_drv_poll line 94
[ 2310.635623] gpio_key_drv.c gpio_key_drv_poll line 94
timeout[ 2310.649112] gpio_key_drv.c gpio_key_drv_poll line 94

[ 2315.650668] gpio_key_drv.c gpio_key_drv_poll line 94
timeout[ 2315.664170] gpio_key_drv.c gpio_key_drv_poll line 94

[ 2320.667136] gpio_key_drv.c gpio_key_drv_poll line 94
timeout[ 2320.677760] gpio_key_drv.c gpio_key_drv_poll line 94

[ 2325.680857] gpio_key_drv.c gpio_key_drv_poll line 94
timeout[ 2325.693654] gpio_key_drv.c gpio_key_drv_poll line 94

[ 2330.695364] gpio_key_drv.c gpio_key_drv_poll line 94
timeout[ 2330.708859] gpio_key_drv.c gpio_key_drv_poll line 94

[ 2335.710210] gpio_key_drv.c gpio_key_drv_poll line 94
timeout[ 2335.723759] gpio_key_drv.c gpio_key_drv_poll line 94

[ 2339.986894] RTL871X: RTW_ADAPTIVITY_EN_[ 2339.990670] AUTO, chplan:0x20, Regulation:3,3
[ 2339.995176] RTL871X: RTW_ADAPTIVITY_MODE_[ 2340.002626] NORMAL
[ 2340.725878] gpio_key_drv.c gpio_key_drv_poll line 94
timeout[ 2340.744492] gpio_key_drv.c gpio_key_drv_poll line 94

[ 2342.495773] RTL871X: nolinked power save leave
[ 2344.372445] RTL871X: nolinked power save enter
[ 2345.747296] gpio_key_drv.c gpio_key_drv_poll line 94
timeout[ 2345.760830] gpio_key_drv.c gpio_key_drv_poll line 94

[ 2350.765047] gpio_key_drv.c gpio_key_drv_poll line 94
timeout[ 2350.776541] gpio_key_drv.c gpio_key_drv_poll line 94

[ 2355.780892] gpio_key_drv.c gpio_key_drv_poll line 94
timeout[ 2355.791770] gpio_key_drv.c gpio_key_drv_poll line 94

[ 2360.795559] gpio_key_drv.c gpio_key_drv_poll line 94
timeout[ 2360.806881] gpio_key_drv.c gpio_key_drv_poll line 94

[ 2365.807639] gpio_key_drv.c gpio_key_drv_poll line 94
timeout[ 2365.821079] gpio_key_drv.c gpio_key_drv_poll line 94

[ 2370.823857] gpio_key_drv.c gpio_key_drv_poll line 94
timeout[ 2370.837494] gpio_key_drv.c gpio_key_drv_poll line 94

[ 2375.841352] gpio_key_drv.c gpio_key_drv_poll line 94
timeout[ 2375.852185] gpio_key_drv.c gpio_key_drv_poll line 94




*/