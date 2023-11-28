#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h> 


void testFunc()
{
    printf("hello world!\n");
}

int main()
{
    int ret;
    int procNum = 2;
    int loopNum = 3;
    int i = 0, j = 0;
    pid_t pid;

    for (i = 0; i < procNum; i++)
    {
        pid = fork();

        if (pid == 0)
        {
            for (j = 0; j < loopNum; j++)
            {
                testFunc();
            }
            printf("PID:%d exit!\n", getpid());
            exit(0);
        }
    }

    // 检测到所有子进程退出，父进程才退出
    while (1)
    {
        ret = wait(NULL);
        if (ret == -1)
        {
            if (errno == EINIT)
            {
                continue;
            }
            break;
        }
    }
    return 0;
}
