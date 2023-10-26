/**********************************************************************
 * 功能描述： 1.在父进程中创建一个子进程，
              2.父进程执行完后，将变量process_inter赋值为1；
              3.子进程判断process_inter为1则执行后面的打印语句，否则不执行。

 ***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    pid_t pid;
    int process_inter = 0;

    pid = fork(); // 创建子进程

    if (pid == 0)
    { // 子进程和父进程不共享 process_inter 变量，只是copy了一份
        int i = 0;
        while (process_inter == 0)
        {
            usleep(1000);
            printf(".%d",process_inter);
        };
        for (i = 0; i < 5; i++)
        {
            usleep(100);
            printf("this is child process i=%d\n", i);
        }
    }

    if (pid > 0)
    { // 父进程
        int i = 0;
        for (i = 0; i < 5; i++)
        {
            usleep(100);
            printf("this is parent process i=%d\n", i);
        }
        process_inter = 1;
        printf("parent process end! process_inter = %d \n", process_inter);
    }

    while (1)
        ;
    return 0;
}

/*


this is parent process i=0
this is parent process i=1
this is parent process i=2
this is parent process i=3
this is parent process i=4
parent process end! process_inter = 1
.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0
.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0

*/