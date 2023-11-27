#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

/*
vfork：
1〉在fork还没实现copy on write之前。
Unix设计者很关心fork之后立刻执行exec所造的地址空间浪费，
所以引入了vfork,系统调用。
2) vfork,有个限制，子进程必须立刻执行_exit或者exec函数。
即使fork实现了copy on write，效率也没有vfork高
，但是我们不推荐使用vfork，因为几子每一个vfork的实现，
都或多或少存在一定的问题。,


结论:,
1:  fork子进程拷贝父进程的数据段。
    vfork子进程与父进程共享数据段;
2:  fork父、子进程的执行次序不确定、
    vfork:子进程先运行结束，父进程后运行;·


注意，vfork 拉起进程后，代码段，数据段全部被新程序替换
但是 pid 保持不变
*/

int main(int argc, char *argv[])
{
    int num = 0;
    pid_t pid = vfork();

    if (pid == 0)
    {
        // 子进程
        printf("PID:%d\tChild process\n", getpid());
        // sleep(5);
         num = 11;
        printf("Child num = %d \n", num);
        _exit(0); // 子进程调用_exit函数终止
    }
    else if (pid > 0)
    {
        // 父进程
        printf("PID:%d\tParent process\n", getpid());
       
        printf("Parent num = %d \n", num);
    }

    printf("end...\n");
    return 0;
}

/*
s$ ./a.out
PID:50006       Child process
Child num = 11
PID:50005       Parent process
Parent num = 11
end...




*/