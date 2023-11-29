
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

/*


*/

void printsigset(const sigset_t *set)
{

	int i;
	for (i = 1; i < NSIG; i++)
	{
		if (sigismember(set, i))
			printf("1");
		else
			printf("0");
	}
	printf("\n");
}

void handler(int num)
{
	printf("handler start num = %d..\n ", num);

	if (num == SIGINT)
	{
		printf("pid = %d\trecv SIGINT(%d)..\n ", getpid(), num);
	}
	if (num == SIGQUIT)
	{
		printf("pid = %d\trecv SIGQUIT(%d)..\n ", getpid(), num);
		sigset_t uset;
		sigemptyset(&uset);
		sigaddset(&uset, SIGINT);
		// ctrl + \触发 SIGQUIT 信号
		// 解除阻塞
		sigprocmask(SIG_UNBLOCK, &uset, NULL);
		printf("pid = %d\trecv SIGQUIT(%d)end ..\n ", getpid(), num);
	}
}

int main(int argc, char *argv[])
{
	sigset_t pset; // 打印信号集
	sigset_t bset; // 设置阻塞信号集

	sigemptyset(&bset);		  // 清空一个信号集
	sigaddset(&bset, SIGINT); // 将指定信号加入到信号集

	if (SIG_ERR == signal(SIGINT, handler))
	{
		perror("signal");
	}
	if (SIG_ERR == signal(SIGQUIT, handler))
	{
		perror("signal");
	}

	// 设置 SIGINT 为阻塞，接受信号也不会抵达
	sigprocmask(SIG_BLOCK, &bset, NULL);
	printf("Total number of signals supported by the system: %d\n", NSIG);
	while (1)
	{
		sigpending(&pset); // 获取 pending 未决字信息
		printsigset(&pset);
		sleep(1);
	}

	printf("main end ..\n ");

	return 0;
}

/*

可以看到，优先处理 SIGQUIT 才是 SIGINT
说明将 SIGINT 设备为非阻塞后，立刻被执行了


Total number of signals supported by the system: 65
0000000000000000000000000000000000000000000000000000000000000000
0000000000000000000000000000000000000000000000000000000000000000
^C0100000000000000000000000000000000000000000000000000000000000000
0100000000000000000000000000000000000000000000000000000000000000

^\handler start num = 3..
 pid = 53852    recv SIGQUIT(3)..
 handler start num = 2..
 pid = 53852    recv SIGINT(2)..
 pid = 53852    recv SIGQUIT(3)end ..



*/