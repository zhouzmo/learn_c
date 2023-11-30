
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

/*
	测试 signal 返回值
*/

void handler(int num)
{
	printf("handler start num = %d..\n ", num);
	if (num == SIGQUIT)
	{
	}
}

int main(int argc, char *argv[])
{

	char tmpchar;
	// 返回原来处理
	__sighandler_t old = signal(SIGINT, handler); // SIGINT = ctrl+c ,
	if (SIG_ERR == old)
	{
		perror("signal");
	}

	printf("if input q，恢复 SIGINT 默认行为..\n");
	while ('q' != (tmpchar = getchar()))
	{
		pause();
	}

	signal(SIGINT, old); // SIGINT 恢复默认行为

	while (1)
	{
		pause();
	}
	printf("main end..\n");

	return 0;
}

/*


*/