/**********************************************************************
 * 功能描述： 1.捕捉终端发送过来的信号
 ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

void handler(int signo)
{
	switch (signo)
	{
	case SIGINT:
		printf("I have got SIGINT\n");
		break;

	case SIGQUIT:
		printf("I have got SIGQUIT\n");
		break;

	default:
		printf("don't respond to this signal[%d]\n", signo);
		exit(0);
	}
}

int main(int argc, char *argv[])
{

	char buf[1024];

	signal(SIGINT, handler);
	signal(SIGQUIT, handler);

	while (1)
	{
		memset(buf, 0, 1024);
		printf("please input message:");
		scanf("%s", buf);
		printf("message lenght:%ld\n", strlen(buf));
		if (0 == strcmp(buf, "sigint"))
		{
			kill(getpid(), SIGINT);
		}
		if (0 == strcmp(buf, "sigquit"))
		{
			kill(getpid(), SIGQUIT);
		}
	}

	while (1)
		pause();
	return 0;
}

/*
$ gcc send_single.c

$ ./a.out
please input message:sigint
message lenght:6
I have got SIGINT
please input message:sigquit
message lenght:7
I have got SIGQUIT
please input message:


*/