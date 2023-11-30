#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

void testFunc()
{
    printf("hello world!\n");
}

int main()
{
    int ret;
    int procNum = 3;
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
    // while (1)
    // {
    //     ret = wait(NULL);
    //     if (ret == -1)
    //     {
    //         if (errno == EINTR)// 判断是否是信号中断导致
    //         {
    //             continue;
    //         }
    //         break;
    //     }
    // }

    int mypid = 0;
    while((mypid = waitpid(-1,NULL,WNOHANG))>0){
         printf("退出子进程得PID:%d exit!\n",mypid);
    }
    return 0;
}
