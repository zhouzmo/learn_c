#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <signal.h>

int main(int argc, char *argv[])
{

    char buf[2];
    int fd;
    int ret;
    int count = 0;
    fd = open("1.txt", O_RDWR);
    if (fd < 0)
    {
        perror("open");
        return 0;
    }
    // 忽略 SIGINT SIGQUIT
    signal(SIGINT, SIG_IGN);  // ctrl c
    signal(SIGQUIT, SIG_IGN); // ctrl "\"
    printf("屏蔽 signal开始读\n");

    while ((ret = read(fd, buf, sizeof(buf))) != 0)
    {
        count += ret;
        printf("%s", buf);
        sleep(1);
    }
    printf("\n读取到 %d\n", count);

    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);

    while (1)
        pause();
    return 0;
}