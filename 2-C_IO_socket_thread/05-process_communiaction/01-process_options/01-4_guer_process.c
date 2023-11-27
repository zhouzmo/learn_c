#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

/*
孤儿进程：
孤儿进程是指一个父进程终止或者被操作系统收回后，
子进程仍然在运行的进程。这种情况可能会导致资源
泄露和进程管理的混乱。为了避免孤儿进程的问题，
操作系统会将孤儿进程的父进程设置为init进程（进
程ID为1），init进程负责收养孤儿进程并进行管理。
*/

int main(int argc, char *argv[])
{
    pid_t pid;
    // signal(SIGCHLD, SIG_IGN); // 父进程自己处理僵尸进程
    int num = 0;

    pid = fork(); // 创建子进程

    if (pid == 0)
    {
        // 子进程
        printf("Child process\n");
        exit(0);
    }
    else if (pid > 0)
    {
        // 父进程
        printf("Parent process\n");
        sleep(2);   // 等待子进程终止
        wait(NULL); // 回收子进程
        printf("Child process has been terminated\n");
    }

    while (1)
        ;

    return 0;
}
/*


父进程进程退出，子进程变为孤儿进程
book      48619      1 85 04:00 pts/3    00:00:03 ./a.out



*/