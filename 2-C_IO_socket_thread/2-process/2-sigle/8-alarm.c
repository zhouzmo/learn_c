
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

/*
alarm(1)是一个系统调用函数，用于设置定时器，以在指定的时间间隔（以秒为单位）后触发一个SIGALRM信号。该函数常用于实现定时操作或超时处理。

alarm()函数的原型如下：
#include <unistd.h>

unsigned int alarm(unsigned int seconds);
seconds参数指定了定时器的时间间隔，即秒数。当调用alarm(seconds)时，操作系统会设置一个定时器，在指定的时间间隔后发送一个SIGALRM信号给当前进程。如果之前已经设置了定时器，则新的调用会覆盖之前的设置，以新的时间间隔为准。

alarm()函数的返回值是上一次设置的定时器剩余的时间，即距离触发下一个SIGALRM信号的剩余时间。如果之前没有设置定时器或之前的定时器已经触发过，则返回值为0。
*/

void handler(int num)
{
	printf("handler start num = %d..\n ", num);

	if (num == SIGALRM)
	{
		printf("pid = %d\trecv SIGALRM..\n ", getpid());
	}
}

int main(int argc, char *argv[])
{
	int ret;

	if (SIG_ERR == signal(SIGALRM, handler))
	{
		perror("signal");
	}

	alarm(1);

	while (1)
	{
		pause();
		printf("pause return ..\n ");
	}

	printf("main end ..\n ");

	return 0;
}

/*

 handler start num = 14..
 pid = 52905    recv SIGALRM..
 pause return ..
^C



*/