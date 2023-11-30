
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

/*
	测试 sleep
*/

void handler(int num)
{
	printf("handler start num = %d..\n ", num);
	if (num == SIGUSR1)
	{
		printf("pid = %d\trecv SIGUSR1..\n ", getpid());
	}
}

int main(int argc, char *argv[])
{
	int ret;

	if (SIG_ERR == signal(SIGINT, handler))
	{
		perror("signal");
	}

	//  ret = sleep(100); // 可中断睡眠
	ret = 10;
	do
	{
		ret = sleep(ret); // 睡够时间
	} while (ret > 0);
	printf("main end ret = %d..\n ", ret);

	return 0;
}

/*



^Chandler start num = 2..
^C handler start num = 2..
^C handler start num = 2..
^C handler start num = 2..
^C handler start num = 2..
sdasd
csac
 main end ret = 0..

*/