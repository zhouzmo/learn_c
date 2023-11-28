
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

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
	signal(SIGINT, handler); // SIGINT = ctrl+c ,

	printf("if input q，恢复 SIGINT 默认行为..\n");
	while ('q' != (tmpchar = getchar()))
	{
		pause();
	}

	signal(SIGINT, SIG_DFL); // SIGINT 恢复默认行为

	while (1)
	{
		pause();
	}
	printf("main end..\n");

	return 0;
}

/*
if input q，恢复 SIGINT 默认行为..
^Chandler start num = 2..
^C handler start num = 2..
^C handler start num = 2..
^C handler start num = 2..
^C handler start num = 2..
^C handler start num = 2..
^C handler start num = 2..
^C handler start num = 2..
^C handler start num = 2..
^C handler start num = 2..
q
^C

*/