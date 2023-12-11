#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
/*
`pipe()` 函数是一个用于创建管道的系统调用。下面是对 `pipe()` 函数的函数文档的简要说明：

**函数原型：**
#include <unistd.h>
int pipe(int pipefd[2]);

**函数描述：**
`pipe()` 函数用于创建一个无名管道，该管道可以用于在相关进程之间传递数据。它创建了一个管道文件描述符数组 `pipefd`，其中 `pipefd[0]` 表示管道的读端，`pipefd[1]` 表示管道的写端。

**参数：**
- `pipefd`：一个包含两个整数的数组，用于存储管道的读端和写端的文件描述符。

**返回值：**
- 成功：返回 0。
- 失败：返回 -1，并设置 `errno` 来指示错误的原因。

**错误情况：**
- `EFAULT`：`pipefd` 数组指针无效。
- `EMFILE`：已达到进程可打开的文件描述符的最大数目。
- `ENFILE`：系统已达到打开文件的最大数目。
- `ENOMEM`：内存不足，无法创建管道。



*/

/*
在管道的读写规则方面，以下是一些重点内容：

1. 当没有数据可读时：
   - 如果未启用 `O_NONBLOCK`（非阻塞）选项，`read` 调用会阻塞，进程会暂停执行，直到有数据可读。
   - 如果启用了 `O_NONBLOCK` 选项，`read` 调用会立即返回-1，`errno` 值为 `EAGAIN`。

2. 当管道满时：
   - 如果未启用 `O_NONBLOCK` 选项，`write` 调用会阻塞，直到有进程读取数据使管道有空间可写。
   - 如果启用了 `O_NONBLOCK` 选项，`write` 调用会立即返回-1，`errno` 值为 `EAGAIN`。

3. 当管道被写满时：
   - 如果未启用 `O_NONBLOCK` 选项，`write` 调用会阻塞，直到管道中有空间可写。
   - 如果启用了 `O_NONBLOCK` 选项，`write` 调用会立即返回-1，`errno` 值为 `EAGAIN`。

4. 如果所有管道写端对应的文件描述符被关闭，则 `read` 返回0。
5. 如果所有管道读端对应的文件描述符被关闭，则 `write` 操作会产生 `SIGPIPE` 信号。
6. 当要写入的数据量不大于 `PIPE_BUF` 时，Linux 保证写入的原子性。
7. 当要写入的数据量大于 `PIPE_BUF` 时，Linux 不再保证写入的原子性。


*/

int main(void)
{
    int ret;
    int pipefd[2];
    char readbuf[1909] = {0};
    pid_t pid;

    ret = pipe(pipefd);
    if (ret < 0)
    {
        perror("pipe");
        exit(0);
    }
    /* 将管道读端的文件描述符属性修改为非阻塞模式，read 返回 -1*/
    int flags = fcntl(pipefd[0], F_GETFL); // 获取管道读端的文件描述符属性
    flags |= O_NONBLOCK;                   // 设置非阻塞标志
    ret = fcntl(pipefd[0], F_SETFL, flags);
    if (ret == -1)
    {
        perror("fcntl");
        exit(0);
    }

    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(0);
    }

    if (pid == 0)
    {
        close(pipefd[0]); // 关闭子进程的读端

        // sleep(3); // 子进程不写数据，父进程阻塞读

        // 向管道写入数据
        // write(pipefd[1], "hello,world", 6);

        // printf("child quitting\n");

        close(pipefd[1]); // 关闭子进程的写端
        exit(0);
    }
    else
    {
        close(pipefd[1]); // 关闭父进程的写端
        sleep(3);         /* 所有写端关闭，read 将 返回 0*/
        // 从管道读取数据
        int n = read(pipefd[0], readbuf, sizeof(readbuf));
        printf("Received(size=%d): %s\n", n, readbuf);

        close(pipefd[0]); // 关闭父进程的读端
        wait(NULL);       // 等待子进程退出
        printf("parent quitting\n");
    }

    return 0;
}