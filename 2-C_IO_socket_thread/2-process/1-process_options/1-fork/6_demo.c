#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{

    int fd;

    fd = open("1.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd == -1)
    {
        perror("zmo open err");
    }

    pid_t pid;

    pid = fork();
    if (pid == 0)
    {
        // 子进程
        printf("PID:%d\tthis is child  process \n", getpid());

        write(fd, "child\n", 6);
        close(fd); // 需要 close ，有文件锁在
    }
    else if (pid > 0)
    {
        // 父进程
        printf("PID:%d\tthis is parent  process \n", getpid());
        write(fd, "parent\n", 7);
        close(fd);
    }

    printf(" pid_t pid=%d int fd=%d\t \n", pid,fd);
    printf("PID %d end...\n", getpid());
    // while(1);
    return 0;
}
/*




*/