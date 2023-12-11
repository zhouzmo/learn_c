#include <stdio.h>
#include <unistd.h>

/*
dup2(int oldfd, int newfd)：

dup2() 函数复制 oldfd 的文件描述符，并将其复制到 newfd 指定的文件描述符。
如果 newfd 已经打开，则 dup2() 将首先关闭 newfd，然后再复制 oldfd。
复制成功时，返回新的文件描述符；复制失败时，返回-1，并设置 errno 表示错误的原因。

*/

/*
wc -w 是一个常用的命令行工具，用于统计文件中的单词数量。
它会计算给定文件中的单词个数，并将结果输出到标准输出。
ls | wc -w   “|” 就是管道运算符

下面是该例子的实现
子进程先将 标准输出 变为管道写端 pipefd[1] 
在通过 execlp 执行 ls 命令，这时，ls 的结果就会输入到管道里
父进程将 标准输入 变为管道读端 (pipefd[0]
在通过 execlp 执行 wc 命令，这时，wc 的接受数据就从管道中获取



*/

int main(void)
{
    int pipefd[2];
    pid_t pid;

    if (pipe(pipefd) == -1)
    {
        printf("pipe() err.\n");
        return -1;
    }

    pid = fork();
    if (pid == -1)
    {
        printf("fork err.\n");
        return -1;
    }

    if (pid == 0)
    {                     // 子进程
        close(pipefd[0]); // 子进程关闭管道的读端
        // 复制文件描述符pipefd[1]，给标准输出，即execlp的第一个命令输出到管道中
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        execlp("ls", "ls", NULL);
        // 如果替换新的进程映像失败，则会执行下面一句话
        sprintf(stderr, "execute the cmd ls err..\n");
        exit(1);
    }
    else if (pid > 0)
    { // 父进程
        int len;
        char buf[190] = {0};
        close(pipefd[1]);

        // 复制文件描述符pipefd[0]，给标准输入，即execlp的第二个命令从管道中读取
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        len = read(pipefd[0], buf, 100);
        execlp("wc", "wc", "-w", NULL);
        printf("len:%d,buf:%s\n", len, buf);
    }
    wait(NULL);
    printf("parent quit \n");
    return 0;

    return 0;
}