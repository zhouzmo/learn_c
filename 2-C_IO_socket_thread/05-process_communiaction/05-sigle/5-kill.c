
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

/*
当涉及到操作系统中的信号处理和进程管理时，kill函数是一个重要的系统调用函数，用于向指定的进程或进程组发送信号。

kill函数的原型如下：


#include <sys/types.h>
#include <signal.h>

int kill(pid_t pid, int sig);
kill函数接受两个参数：

pid：要发送信号的目标进程的进程ID（PID）。可以使用特定的PID来指定单个进程，也可以使用特殊的PID值来指定进程组或一组进程。
sig：要发送的信号的编号或宏。可以使用预定义的信号宏（如 SIGTERM）或直接使用信号编号。
kill函数的返回值表示函数执行的结果，具体含义如下：

如果成功发送信号，则返回值为0。
如果发送信号失败，则返回值为-1，并且设置errno来指示错误的原因。
*/

void handler(int num)
{
	printf("handler start num = %d..\n ", num);
	if (num == SIGUSR1)
	{
		printf("pid = %d\trecv SIGUSR1..\n ",getpid());
	}
}

int main(int argc, char *argv[])
{

	char tmpchar;
	pid_t pid;

	
	if (SIG_ERR == signal(SIGINT, handler))
	{
		perror("signal");
	}

	if (SIG_ERR == signal(SIGUSR1, handler))
	{
		perror("signal");
	}

	pid = fork();
	if (pid == 0)
	{ // 子进程
		pid = getpid();
		// kill(pid, SIGUSR1); // 父进程发送信号
		// kill(0, SIGUSR1);	// 进程组发送信号
		killpg(getpgrp(), SIGUSR1);	// 进程组发送信号
		exit(0);
	}

	sleep(3);
	printf("main end..\n ");

	return 0;
}

/*


*/