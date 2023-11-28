
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

/*
pause()函数是一个系统调用函数，用于将当前进程置于可中断的睡眠状态，直到接收到一个信号。它会暂停进程的执行，并将CPU资源pause()函数是一个系统调用函数，用于将当前进程置于可中断的睡眠状态，直到接收到一个信号。它会暂停进程的执行，并将CPU资源释放给其他可运行的进程。

pause()函数的原型如下：

c
复制
#include <unistd.h>

int pause(void);
当调用pause()函数时，进程将进入睡眠状态，直到收到一个信号。此时，进程会被挂起，不会继续执行，直到以下两种情况之一发生：

接收到一个信号，其信号处理函数被调用。
接收到一个未被忽略的信号，其默认行为是终止进程。
pause()函数的返回值始终为-1，且设置errno为EINTR（被信号中断）。
*/

void handler(int num)
{
	printf("handler start num = %d..\n ", num);
	if (num == SIGUSR1)
	{
		printf("pid = %d\trecv SIGUSR1..\n ", getpid());
	}
	if (num == SIGINT)
	{
		printf("pid = %d\trecv SIGINT..\n ", getpid());
	}
}

int main(int argc, char *argv[])
{
	int ret;

	if (SIG_ERR == signal(SIGINT, handler))
	{
		perror("signal");
	}

	while (1)
	{
		pause();
		printf("pause return ..\n ");
	}

	printf("main end ..\n ");

	return 0;
}

/*



Chandler start num = 2..
 pid = 52869    recv SIGINT..
 main return ..


^C handler start num = 2..
 pid = 52869    recv SIGINT..
 main return ..



^C handler start num = 2..
 pid = 52869    recv SIGINT..
 main return ..
^Z


*/