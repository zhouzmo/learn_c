
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

/*
sigqueue
发送带参数的信号
*/

void myHandle_forsigaction(int signum, siginfo_t *info, void *context)
{
	if (signum == SIGINT)
	{
		printf("Received a signal: %d\n", signum);
		printf("Data received: %d\n", info->si_value.sival_int);
	}
}

int main(int argc, char *argv[])
{

	pid_t pid;
	int ret;
	struct sigaction act;
	// 如果父进程 SIGINT 回调函数，准备接收带有数据的，需要设置 sa_flags
	act.sa_flags = SA_SIGINFO;

	act.sa_sigaction = myHandle_forsigaction;
	sigemptyset(&act.sa_mask);

	if (sigaction(SIGINT, &act, NULL) < 0)
	{
		perror("sigaction error");
		exit(EXIT_FAILURE);
	}

	pid = fork();
	if (pid == -1)
	{
		perror("fork error");
	}

	if (pid == 0)
	{
		printf("Child process...\n");

		// ret = kill(getppid(), SIGINT);

		// 发送带数据的信号
		union sigval mysigcal;
		mysigcal.sival_int = 222;
		ret = sigqueue(getppid(), SIGINT, mysigcal);
		printf("sigqueue ret =%d\n", ret);
		exit(0);
	}

	// Parent process
	while (1)
		;
	printf("Signal received. Exiting...\n");

	return 0;
}
/*


*/