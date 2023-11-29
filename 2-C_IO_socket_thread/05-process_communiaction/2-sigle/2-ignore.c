
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{

	pid_t pid;

	signal(SIGCHLD, SIG_IGN); // 父进程自己处理僵尸进程

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
	}
	if (pid == 0)
	{ // 子进程
		printf("child...\n");
		exit(0);
	}

	while (1)
	{
		pause();
	}
	printf("main end..\n");

	return 0;
}
