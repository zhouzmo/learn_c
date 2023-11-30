
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

/*
可重入函数是指在多线程或多任务环境下可以安全调用的函数，即多个线程或任务可以同时调用该函数而不会出现数据竞争或不一致的情况。可重入函数使用的是局部变量而不是全局或静态变量，或者对于共享变量的访问进行了适当的同步控制。

在信号处理函数中，由于信号的异步性质，可能会中断正在执行的代码而立即调用信号处理函数。因此，在信号处理函数中应该尽量使用可重入函数，以确保处理过程的安全性和可靠性。

不可重入函数是指在多线程或多任务环境下调用可能导致数据竞争或不一致性的函数。不可重入函数通常使用全局变量、静态变量、动态内存分配（如`malloc()`和`free()`）或标准I/O函数等可能引起竞态条件或数据共享问题的操作。
*/

typedef struct Stu
{
	int age;
	int num;

} Stu;

Stu g_stu;

void printStuInfo(Stu stu)
{
	printf("Stu age=%d  num = %d..\n ", stu.age, stu.num);
}

void handler(int num)
{
	// printf("handler start num = %d..\n ", num);
	printStuInfo(g_stu);// 可能出现
	// unsafe_fun();
	alarm(1);
}

int main(int argc, char *argv[])
{
	Stu stu1, stu2;
	stu1.age = 11;
	stu1.num = 111;
	stu2.age = 22;
	stu2.num = 222;

	if (SIG_ERR == signal(SIGALRM, handler))
	{
		perror("signal");
	}

	alarm(1);

	while (1)
	{
		g_stu = stu1;
		g_stu = stu2;
	}

	printf("main end ..\n ");

	return 0;
}

/*

 Stu age=22  num = 222..
 Stu age=22  num = 222..
 Stu age=11  num = 111..
 Stu age=22  num = 222..


*/