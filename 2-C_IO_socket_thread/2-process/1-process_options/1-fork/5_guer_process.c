#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

/*
孤儿进程：
孤儿进程是指一个父进程终止或者被操作系统收回后，
子进程仍然在运行的进程。这种情况可能会导致资源
泄露和进程管理的混乱。
为了避免孤儿进程的问题，
操作系统会将孤儿进程的父进程设置为init进程（进
程ID为1），init进程负责收养孤儿进程并进行管理。
*/

int main(int argc, char *argv[])
{
    pid_t pid;
    signal(SIGCHLD, SIG_IGN); // 方法二：父进程自己处理僵尸进程
    int num = 0;

    pid = fork(); // 创建子进程

    if (pid == 0)
    {
        // 子进程
        printf("Child process：%d\n", getpid());

        // sleep(2);
        exit(0); // 子进程先死，父进程不处理，编程僵尸进程
    }
    else if (pid > 0)
    {
        // 父进程
          
        printf("Parent process:%d\n", getpid());
        sleep(2);
        // exit(0); // 父进程先死，子进程变为孤儿进程

        // wait(NULL); // 方法一:回收所有子进程
        printf("Child process has been terminated\n");
    }

    while (1)
        ;

    return 0;
}
/*
父进程进程退出，子进程变为孤儿进程
$ ps -ef |grep a.out
book       6013      1 90 14:27 pts/0    00:00:17 ./a.out
book       6016   3005  0 14:27 pts/0    00:00:00 grep --color=auto a.out

*/

/*
子进程先死，父进程不处理，变为僵尸进程,父进程结束，他也结束

book       6040   3005 93 14:31 pts/0    00:00:32 ./a.out
book       6041   6040  0 14:31 pts/0    00:00:00 [a.out] <defunct>


*/