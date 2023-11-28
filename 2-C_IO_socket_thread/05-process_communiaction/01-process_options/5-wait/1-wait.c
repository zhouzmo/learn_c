
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

/*

pid_t wait(int *wstatus);

pid_t waitpid(pid_t pid, int *wstatus, int options);

*/

int main(int argc, char *argv[])
{

    pid_t pid;

    pid = fork();
    if (pid == 0)
    {
        sleep(3);
        printf("child\n");
        // exit(100);
        abort(); // 非正常退出
    }

    printf("parent\n");

    int ret;
    int status;
    ret = wait(&status);// 父进程暂停
    printf("pid = %d\tret=0x%x\n", getpid(), ret);

    /*
    WIFEXITED(status)
    如果子进程正常结束，返回一个非 0 值，
    */
    if (WIFEXITED(status)) // wait if exit end等待正常结束
    {
        printf("进程退出：%d\n", WEXITSTATUS(status)); // 进程退出：6
    }

    /*
   WIFEXITED(status)
   如果子进程正常结束，返回一个非 0 值，
   */
    if (WIFEXITED(status)) // wait if exit end等待正常结束
    {
        printf("进程退出：%d\n", WEXITSTATUS(status)); // WIFEXITED 非0，返回子进程退出码
    }

    if (WIFSIGNALED(status)) // wait if singal end等待信号结束
    {
        printf("进程退出：%d\n", WTERMSIG(status)); // 进程退出：6
    }

    return 0;
}

/*

*/