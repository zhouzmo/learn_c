
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

/*



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
