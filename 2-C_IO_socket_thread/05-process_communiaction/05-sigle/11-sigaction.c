
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

/*

sigaction函数用于注册信号处理函数，并可以指定其他选项来控制信号的处理方式。它的原型如下：


int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
参数解释如下：

signum：要注册处理函数的信号编号。
act：一个指向struct sigaction结构的指针，其中包含了对指定信号的处理方式和其他选项。
oldact：一个指向struct sigaction结构的指针，用于保存原来对相应信号的处理方式（可选参数）。

struct sigaction结构体定义如下：
struct sigaction {
	void (*sa_handler)(int);                                // 简单信号处理函数
	void (*sa_sigaction)(int, siginfo_t *, void *);         // 带有附加信息的信号处理函数
	sigset_t sa_mask;                                       // 在信号处理函数运行期间需要阻塞的信号集
	int sa_flags;                                           // 控制信号处理的选项
	void (*sa_restorer)(void);                              // 已废弃，无需使用
};
其中，sa_handler字段是一个指向简单信号处理函数的指针，它接收一个整数参数表示收到的信号编号。而sa_sigaction字段是一个指向带有附加信息的信号处理函数的指针，它接收三个参数：信号编号、指向siginfo_t结构的指针（包含了关于信号的更多信息）和一个void指针（不常用）。

除了处理函数之外，struct sigaction结构还包含了其他字段。sa_mask字段用于指定在信号处理函数执行期间需要阻塞的信号集。sa_flags字段用于控制信号处理的选项，例如设置SA_RESTART选项可以在系统调用被中断后自动重启。sa_restorer字段已废弃，不再使用。

函数的返回值为0表示成功，-1表示失败。
*/

void myHandle_forsigaction(int signum, siginfo_t *info, void *context)
{
	printf("Received a signal: %d\n", signum);
	printf("Data received: %d\n", info->si_value.sival_int);
}

int main(int argc, char *argv[])
{

	pid_t pid;
	int ret;
	struct sigaction act;

	act.sa_sigaction = myHandle_forsigaction;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;

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