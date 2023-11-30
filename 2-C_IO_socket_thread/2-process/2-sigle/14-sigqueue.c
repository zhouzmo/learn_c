
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

/*
 发送 3 次非实时信号 success
发送 3 次实时信号 success
发送  SIGUSR1 信号解除阻塞
现象：
执行 3 次实时信号，1 次非实时信号
*/
//
void myHandle_forsigaction(int signum, siginfo_t *info, void *context)
{
	printf("handler start num = %d..\n ", signum);

	if (signum == SIGINT)
	{
		printf("pid = %d\trecv SIGINT value= %d..\n ", getpid(), info->si_value.sival_int);
	}
	if (signum == SIGRTMIN)
	{
		printf("pid = %d\trecv SIGRTMIN value= %d ..\n ", getpid(), info->si_value.sival_int);
	}
	if (signum == SIGUSR1)
	{
		sigset_t bset;
		sigemptyset(&bset);		  // 清空一个信号集
		sigaddset(&bset, SIGINT); // 将指定信号加入到信号集
		sigaddset(&bset, SIGRTMIN);
		sigprocmask(SIG_UNBLOCK, &bset, NULL);

		printf("pid = %d\trecv SIGUSR1 value= %d ..\n ", getpid(), info->si_value.sival_int);
	}
}
int main(int argc, char *argv[])
{

	pid_t pid;
	int ret;
	int i;
	struct sigaction act;
	act.sa_flags = SA_SIGINFO;

	// act.sa_sigaction = myHandle_forsigaction;
	act.sa_handler = myHandle_forsigaction;

	// 注册 非实时 信号处理函数 0-31
	if (sigaction(SIGINT, &act, NULL) < 0)
	{
		perror("sigaction error");
		exit(EXIT_FAILURE);
	}
	// 注册 实时 信号处理函数 32-63的信号被称为实时信号
	if (sigaction(SIGRTMIN, &act, NULL) < 0)
	{
		perror("sigaction error");
		exit(EXIT_FAILURE);
	}
	// 注册 用户自定义SIGUSR1 信号处理函数
	if (sigaction(SIGUSR1, &act, NULL) < 0)
	{
		perror("sigaction error");
		exit(EXIT_FAILURE);
	}

	sigset_t bset;
	sigemptyset(&bset);		  // 清空一个信号集
	sigaddset(&bset, SIGINT); // 将指定信号加入到信号集
	sigaddset(&bset, SIGRTMIN);

	// 将 SIGINT SIGRTMIN 设置到本进程的阻塞状态字中
	sigprocmask(SIG_BLOCK, &bset, NULL);

	pid = fork();
	if (pid == 0)
	{
		union sigval mysigval;
		mysigval.sival_int = 111;
		// 发送 3 次 非实时 信号
		for (i = 0; i < 3; i++)
		{
			ret = sigqueue(getppid(), SIGINT, mysigval);
			if (ret < 0)
			{
				perror("sigqueue");
			}
		}
		printf("发送 3 次非实时信号 success \n");

		mysigval.sival_int = 222;
		// 发送 3 次 实时 信号
		// 可靠信号可以发送 ulimit -a   pending signals    次
		for (i = 0; i < 3; i++)
		{
			mysigval.sival_int ++;
			ret = sigqueue(getppid(), SIGRTMIN, mysigval);
			if (ret < 0)
			{
				printf("发送实时信号 error \n");
			}
			else if (ret == 0)
			{
				printf("发送实时信号 success ...\n");
			}
		}
		printf("发送 3 次实时信号 success \n");

		// 发送 SIGUSR1 解除  SIGINT SIGRTMIN  阻塞信号
		kill(getppid(), SIGUSR1);
		printf("发送  SIGUSR1信号 success \n");
		exit(0);
	}

	while (1)
	{
		pause();
	};

	return 0;
}
/*

 发送 3 次非实时信号 success
发送 3 次实时信号 success
发送  SIGUSR1信号 success
handler start num = 10..
 handler start num = 34..
 pid = 54622    recv SIGRTMIN(34)end ..
 handler start num = 34..
 pid = 54622    recv SIGRTMIN(34)end ..
 handler start num = 34..
 pid = 54622    recv SIGRTMIN(34)end ..
 handler start num = 2..
 pid = 54622    recv SIGINT(2)..
 pid = 54622    recv SIGUSR1(10)end ..

*/