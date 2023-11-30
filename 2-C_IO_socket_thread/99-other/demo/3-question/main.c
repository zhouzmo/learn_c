#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
/*


父进程创建两个子进程，称为进程1和进程2。
进程1向进程2发送可靠信号，并传递额外数据为进程1的PID乘以2。
进程2接收可靠信号的值，并发送给父进程，父进程将接收到的值进行打印。
提示:广用sigqueue和sigaction实现

todo
创建 进程1 进程2
父进程将 进程2pid 发送给 进程1
进程1 向 进程2 发送可靠信号
进程2 接收可靠信号的值，并发送给父进程
*/

void myHandle_forsigaction(int signum, siginfo_t *info, void *context)
{
    union sigval mysigval;
    if (signum == SIGRTMIN + 1)
    {
        pid_t pid2 = info->si_value.sival_int;

        printf("pid = %d\trecv SIGRTMIN + 1 value= %d..\n ", getpid(), pid2);
        // 进程1向进程2发送可靠信号，并传递额外数据为进程1的PID乘以2。
        mysigval.sival_int = getpid() * 2;
        if ((sigqueue(pid2, SIGRTMIN + 2, mysigval)) < 0)
        {
            perror("sigqueue");
        }
        printf("send success\n");
    }
    if (signum == SIGRTMIN + 2)
    {
        // 进程2接收可靠信号的值，并发送给父进程，父进程将接收到的值进行打印。
        int value = info->si_value.sival_int;
        printf("pid = %d\trecv  SIGRTMIN + 2 value= %d..\n ", getpid(), value);
        mysigval.sival_int = value;
        if ((sigqueue(getppid(), SIGRTMIN + 3, mysigval)) < 0)
        {
            perror("sigqueue");
        }
        printf("send success\n");
    }
    if (signum == SIGRTMIN + 3)
    {
        // 父进程将接收到的值进行打印。
        int value = info->si_value.sival_int;
        printf("pid = %d\trecv  SIGRTMIN + 3 value= %d..\n ", getpid(), value);
    }
}
int main()
{

    pid_t pid1, pid2;

    int ret;
    int i;
    union sigval mysigval;

    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = myHandle_forsigaction;

    // 注册 非实时 信号处理函数 0-31
    if (sigaction(SIGRTMIN + 2, &act, NULL) < 0)
    {
        perror("sigaction error");
        exit(EXIT_FAILURE);
    }
    // 注册 实时 信号处理函数 32-63的信号被称为实时信号
    if (sigaction(SIGRTMIN + 1, &act, NULL) < 0)
    {
        perror("sigaction error");
        exit(EXIT_FAILURE);
    }
    // 注册 用户自定义SIGUSR1 信号处理函数
    if (sigaction(SIGRTMIN + 3, &act, NULL) < 0)
    {
        perror("sigaction error");
        exit(EXIT_FAILURE);
    }

    pid1 = fork();
    if (pid1 < 0)
    {
        perror("fork");
    }
    else if (pid1 == 0)
    {
        printf("child pid1:%d,\n", getpid());
        pause(); // 挂起，等待父进程给你发 pid2
        exit(1);
    }

    pid2 = fork();
    if (pid2 < 0)
    {
        perror("fork");
    }
    else if (pid2 == 0)
    {
        printf("child pid2:%d,\n", getpid());
        pause(); // 挂起，等待 pid1 给你发 pid2
        exit(1);
    }

    printf("pid:%d,child pid1:%d ,child pid2%d\n", getpid(), pid1, pid2);

    // 父进程将 进程2pid 发送给 进程1
    mysigval.sival_int = pid2;
    ret = sigqueue(pid1, SIGRTMIN + 1, mysigval);
    if (ret < 0)
    {
        perror("sigqueue");
    }
    printf("pid:%d,child pid1:%d ,child pid2%d\n", getpid(), pid1, pid2);

    pid_t retpid;
    int status;
    for (i = 0; i < 2; i++)
    {
        retpid = wait(&status); // 父进程暂停
        printf("阻塞等待到子进程 %d 结束,status = %d,status = 0x%x\n", retpid, status, status);

        if (WIFEXITED(status)) // wait if exit end等待正常结束
        {

            // WEXITSTATUS(status) 返回子进程退出状态的低字节，它的范围在 0-255 之间
            printf("进程退出：status = %d,status = 0x%x\n", WEXITSTATUS(status), WEXITSTATUS(status)); // 进程退出码
        }
        else if (WIFSIGNALED(status)) // wait if signal end等待信号结束
        {
            printf("进程被信号终止：%d\n", WTERMSIG(status)); // 信号编号
        }
    }

    while (1)
    {
        pause();
    }

    return 0;
}
