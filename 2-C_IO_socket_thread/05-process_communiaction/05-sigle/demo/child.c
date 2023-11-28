
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

int main(int argc, char const *argv[])
{
    printf("pid:%d\tchild success\n ", getpid());
    union sigval mysigval;
    mysigval.sival_int = 9999;
    if (sigqueue(getppid(), SIGINT, mysigval) == -1)
    {
        perror("sigqueue");
    }

    return 0;
}