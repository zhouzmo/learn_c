#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

/*
进程一次调用 2 次返回的本质
   linux 内核让两个进程，在各种的内存空间中返回

cpoy 什么？
代码，堆栈，数据段，进程控制PCB


为何在   if (pid == 0) 开始运行子进程？
在 fork 函数前，已经给堆栈赋值，无需再次赋值
*/
int main(int argc, char *argv[])
{
    pid_t pid;
    int num = 0;

    pid = fork(); // 创建子进程

    if (pid == 0)
    { // 子进程
        int i = 0;
        for (i = 0; i < 5; i++)
        {
            usleep(100);
            printf("PID:%d\tPPID:%d\this is child  process i=%d,num = %d\n",
                   getpid(), getppid(), i, num++);
        }
    }

    if (pid > 0)
    { // 父进程
        int i = 0;
        for (i = 0; i < 5; i++)
        {
            usleep(100);
            printf("PID:%d\tthis is parent process i=%d,num = %d\n", getpid(), i, num++);
        }
    }

    if (pid == -1)
    { // 失败
        perror("fork");
    }

    while (1)
        ; // 不让进程结束，以便我们查看进程的一些状态信息
    return 0;
}

/*
s$ ./a.out
PID:47643       this is parent process i=0,num = 0
PID:47644       this is child  process i=0,num = 0
PID:47644       this is child  process i=1,num = 1
PID:47643       this is parent process i=1,num = 1
PID:47643       this is parent process i=2,num = 2
PID:47644       this is child  process i=2,num = 2
PID:47643       this is parent process i=3,num = 3
PID:47644       this is child  process i=3,num = 3
PID:47644       this is child  process i=4,num = 4
PID:47643       this is parent process i=4,num = 4





*/