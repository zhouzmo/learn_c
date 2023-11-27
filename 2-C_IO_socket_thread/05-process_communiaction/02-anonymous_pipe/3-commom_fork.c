/**********************************************************************
  * 功能描述： 1.使用无名管道实现父子进程通信
               2.父进程向管道中写入一个值
               3.子进程从管道中读取该值，如果非零，则执行后面的打印，否则不执行

  ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    pid_t pid;
    char process_share_val = 0;
    int fd[2], ret = 0;

    ret = pipe(fd); // 创建一个无名管道，必须在创建子进程之前
    if (ret < 0)
    {
        printf("create pipe fail\n");
        return -1;
    }
    printf("create pipe success\n");

    pid = fork(); // 创建子进程

    if (pid == 0)
    { // 子进程
        int i = 0;
        read(fd[0], &process_share_val, sizeof(process_share_val)); // 如果管道为空，则休眠等待
        while (process_share_val == 0)
            ;
        for (i = 0; i < 5; i++)
        {
            usleep(100);
            printf("this is child process i=%d\n", i);
        }
    }
    else if (pid > 0)
    { // 父进程
        int i = 0;
        for (i = 0; i < 5; i++)
        {
            usleep(100);
            printf("this is parent process i=%d\n", i);
        }
        process_share_val = 1;
        sleep(2);
        write(fd[1], &process_share_val, sizeof(process_share_val));
    }

    while (1)
    {
        usleep(1000);
        printf(".");
    };
    return 0;
}
/*
create pipe success
this is parent process i=0
this is parent process i=1
this is parent process i=2
this is parent process i=3
this is parent process i=4
this is child process i=0
this is child process i=1
this is child process i=2
this is child process i=3
this is child process i=4
.............
*/