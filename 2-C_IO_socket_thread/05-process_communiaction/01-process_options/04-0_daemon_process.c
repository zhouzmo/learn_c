#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

/*





什么是守护进程
守护进程是在后台运行不受控端控制的进程，通常情况下守护进程在
系统启动时自动运行,
守护进程的名称通常以d结尾，比如sshd、 xinetd、 crond等,

创建守护进程步骤
1.调用fork(),创建新进程，它会是将来的守护进程
2.在父进程中调用exit，保证子进程不是进程组组长,
3.调用setsid创建新的会话期
4.将当前目录改为根目录（如果把当前目录作为守护进程的目录，
当前目录不能被卸载，它作为守护进程的工作目录了。〉
5.将标准输入、标准输出、标准错误重定向到/dev/nulle


*/

//  int daemon(int nochdir, int noclose);

int my_daemon(int nochdir, int noclose)
{
    int i;
    pid_t pid;
    // 1.调用fork(),创建新进程，它会是将来的守护进程
    pid = fork();

    if (pid > 0)
    {
        // 父
        // 2.在父进程中调用exit，保证子进程不是进程组组长,
        exit(0);
    }

    if (pid == 0)
    {
        // 子
    }
    // 3.调用setsid创建新的会话期
    /*
    setsid()函数的主要作用是创建一个新的会话，
    并将调用进程设置为该会话的领导者和新进程组的领导者。
    通过这个函数，进程可以独立于现有会话和进程组，
    成为一个新的会话组的领导者。
    */
    pid = setsid();
    if (pid == -1)
    {
        perror("setsid");
    }

    // 4.将当前目录改为根目录（如果把当前目录作为守护进程的目录，
    // 当前目录不能被卸载，它作为守护进程的工作目录了
    if (nochdir != 0)
    {
        chdir("/");
    }

    if (noclose != 0)
    {
        for (i = 0; i < 3; ++i)
        {
            close(i);
        }
    }

    // 5.将标准输入、标准输出、标准错误重定向到/dev/nulle
    // 将 0 号 fd，指向 /dev/null
    open("/dev/null", O_RDWR);
    dup(0); // 将 0 号fd，赋值给空闲的 fd 1
    dup(0); // 将 0 号fd，赋值给空闲的 fd 2

    while (1)
        ;
    return 0;
}

int main(int argc, char *argv[])
{
    my_daemon(1, 1);
    return 0;
}