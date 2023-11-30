
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

/*
signal 注册信号处理
 kill发送信号
*/


/*

可以通过 kill -l 查看信号种类

SIGABRT：进程异常终止信号。通常由调用abort()函数或发生严重错误时触发。
SIGHUP：终端挂断信号。当用户退出终端或与终端连接丢失时，会向与终端相关的进程发送该信号。
SIGINT：终端中断信号。通常由用户在终端上按下Ctrl+C键时触发，用于请求进程终止运行。
SIGKILL：强制终止信号。该信号无法被忽略或捕获，用于立即强制终止一个进程的执行。
SIGPIPE：管道破裂信号。当向一个没有读取者的管道写入数据时，会触发该信号。
SIGQUIT：终端退出信号。通常由用户在终端上按下Ctrl+\键时触发，用于请求进程终止并生成核心转储文件。
SIGTERM：终止信号。通常用于请求进程正常终止运行。
SIGUSR1：用户定义信号1。可以由进程自定义使用的信号。
SIGUSR2：用户定义信号2。可以由进程自定义使用的信号。
SIGCHLD：子进程停止或退出信号。当子进程停止运行或退出时，会向父进程发送该信号。



*/



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

	// 信号处理
	signal(SIGINT, handler); // ctrl c
	signal(SIGQUIT, handler);// ctrl "\"

	// while (1)
	// {
	// 	memset(buf, 0, 1024);
	// 	printf("please input message:");
	// 	scanf("%s", buf);
	// 	printf("message lenght:%ld\n", strlen(buf));
	// 	if (0 == strcmp(buf, "sigint"))
	// 	{
	// 		// 发送信号
	// 		kill(getpid(), SIGINT);
	// 	}
	// 	if (0 == strcmp(buf, "sigquit"))
	// 	{
	// 		kill(getpid(), SIGQUIT);
	// 	}
	// }

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