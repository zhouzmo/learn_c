
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
`wait`和`waitpid`是在Unix-like操作系统中用于等待子进程结束并获取其状态的系统调用。

**wait函数**（`wait`）：
- 函数原型：`pid_t wait(int *status);`
- 功能：等待任意子进程结束，并获取子进程的退出状态。
- 参数：
  - `status`：用于存储子进程退出状态的指针。如果不关心子进程的退出状态，可以传入`NULL`。
- 返回值：
  - 成功：返回终止子进程的进程ID。
  - 失败：返回-1，并设置errno来指示错误。

**waitpid函数**（`waitpid`）：
- 函数原型：`pid_t waitpid(pid_t pid, int *status, int options);`
- 功能：等待指定的子进程结束，并获取子进程的退出状态。
- 参数：
  - `pid`：指定要等待的子进程的进程ID。
    - `pid > 0`：等待指定进程ID的子进程。
    - `pid == -1`：等待任意子进程。
    - `pid == 0`：等待与调用进程属于同一进程组的任意子进程。
    - `pid < -1`：等待进程组ID等于`pid`绝对值的任意子进程。
  - `status`：用于存储子进程退出状态的指针。如果不关心子进程的退出状态，可以传入`NULL`。
  - `options`：附加选项，用于控制`waitpid`的行为，例如`WNOHANG`、`WUNTRACED`等。如果不需要特殊选项，可以传入0。
- 返回值：
  - 成功：返回终止子进程的进程ID。
  - 失败：返回-1，并设置errno来指示错误。

`wait`和`waitpid`的返回值可以用一些宏函数来解释子进程的终止状态，如下所示：

- `WIFEXITED(status)`：如果子进程正常终止，则为真。
- `WEXITSTATUS(status)`：获取子进程的退出状态。
- `WIFSIGNALED(status)`：如果子进程因信号终止，则为真。
- `WTERMSIG(status)`：获取导致子进程终止的信号编号。
- `WIFSTOPPED(status)`：如果子进程当前已停止，则为真。
- `WSTOPSIG(status)`：获取导致子进程停止的信号编号。

这些函数可以帮助您检查子进程的退出状态，并根据需要采取进一步的操作。


*/

void testFunc()
{
    printf("hello world!\n");
}

int main()
{
    int procNum = 5;
    int loopNum = 5;
    printf("please procNum loopNum\n");
    // scanf("%d", &procNum);
    // scanf("%d", &loopNum);
    int i = 0, j = 0;
    pid_t child_pid;

    for (i = 0; i < procNum; i++)
    {
        child_pid = fork();

        if (child_pid == 0)
        {
            for (j = 0; j < loopNum; j++)
            {
                testFunc();
            }
            printf("PID:%d exit!\n", getpid());
            exit(i * 10); // 不退出将会递归创建子进程
        }
    }

    printf("parent\n");

    pid_t ret;
    int status;
    for (i = 0; i < procNum; i++)
    {
        ret = wait(&status); // 父进程暂停
        printf("阻塞等待到子进程 %d 结束,status = %d,status = 0x%x\n", ret, status, status);

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
}
