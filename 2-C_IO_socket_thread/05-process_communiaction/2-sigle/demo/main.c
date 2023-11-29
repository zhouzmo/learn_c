
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
/*

父进程处理带参数信号
子进程发送带参数信号

*/

void handler(int sig, siginfo_t *info, void *ucontext)
{
    printf("pid:%d start handler\n ", getpid());

    if (sig == SIGINT)
    {
        printf("info= %d\n ", info->si_value.sival_int);
    }
}

int main(int argc, char const *argv[])
{
    int ret;
    pid_t pid;
    struct sigaction mysigaction;
    mysigaction.sa_sigaction = handler;
    mysigaction.sa_flags = SA_SIGINFO;
    // 1、sigaction 注册带参信号处理函数
    if (sigaction(SIGINT, &mysigaction, NULL) == -1)
    {
        perror("sigaction");
    }

    // fork 子进程
    pid = fork();
    if (pid == -1)
    {
        perror("fork");
    }
    if (pid == 0)
    {
        printf("pid:%d\tchild\n ", getpid());
        // exce 发送程序
        if (execvp("./child.out", NULL) == -1)
        {
            perror("execve");
        }
    }

    while (1)
        ;

    return 0;
}