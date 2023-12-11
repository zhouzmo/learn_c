#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
/*
6. 当要写入的数据量不大于 `PIPE_BUF` 时，Linux 保证写入的原子性。
7. 当要写入的数据量大于 `PIPE_BUF` 时，Linux 不再保证写入的原子性。


实验：
两个子进程 AB，向管道内部写输入，父进程读数据
现象：
A B 发现数据 AB 存在交替写现象，不能保证原子性操作

n=14 pid=79120 read 4096 bytes from pipe buf[4095]=A
n=15 pid=79120 read 4096 bytes from pipe buf[4095]=A
n=16 pid=79120 read 4096 bytes from pipe buf[4095]=A
n=17 pid=79120 read 4096 bytes from pipe buf[4095]=B
n=18 pid=79120 read 4096 bytes from pipe buf[4095]=A
n=19 pid=79120 read 4096 bytes from pipe buf[4095]=B
n=20 pid=79120 read 4096 bytes from pipe buf[4095]=B
n=21 pid=79120 read 4096 bytes from pipe buf[4095]=B
n=22 pid=79120 read 4096 bytes from pipe buf[4095]=B



*/
#define ERR_EXIT(m)         \
    do                      \
    {                       \
        perror(m);          \
        exit(EXIT_FAILURE); \
    } while (0)

#define TEST_SIZE (68 * 1024 + 1)

int main(void)
{
    char abuf[TEST_SIZE];
    char bbuf[TEST_SIZE];
    memset(abuf, 'A', sizeof(abuf));
    memset(bbuf, 'B', sizeof(bbuf));

    int pipefd[2];
    int ret = pipe(pipefd);
    if (ret == -1)
        ERR_EXIT("pipe error");

    pid_t pid;
    pid = fork();
    if (pid == -1)
        ERR_EXIT("fork error");

    if (pid == 0)
    {
        // 子进程A写入数据
        close(pipefd[0]);
        ret = write(pipefd[1], abuf, sizeof(abuf));
        printf("A pid=%d write %d bytes to pipe\n", getpid(), ret);
        exit(0);
    }

    pid = fork();
    if (pid == -1)
        ERR_EXIT("fork error");

    if (pid == 0)
    {
        // 子进程B写入数据
        close(pipefd[0]); // 首先关闭读
        ret = write(pipefd[1], bbuf, sizeof(bbuf));
        printf("B pid=%d write %d bytes to pipe\n", getpid(), ret);
        exit(0);
    }

    // 父进程读取数据
    close(pipefd[1]); // 首先关闭写
    sleep(1);

    int fd = open("test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
        ERR_EXIT("open error");

    char buf[1024 * 4] = {0};
    int n = 1;
    while (1)
    {
        // 父进程 4k 4k 的读数据，发现 AB 进程是交叉写数据到管道内部
        // 多个进程往管道里面，写数据
        ret = read(pipefd[0], buf, sizeof(buf));
        if (ret == 0)
            break;
        printf("n=%d pid=%d read %d bytes from pipe buf[4095]=%c\n", n, getpid(), ret, buf[4095]);
        write(fd, buf, ret);
        n++;
    }

    close(fd);
    return 0;
}

/*




*/