#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char *argu[])
{
      printf("start...\n");
    // 建立管道文件
    mkfifo("fifop", 0666);
    int infd;
    // 按只读方式打开现有文件
    infd = open("./1.txt", O_RDONLY);
    if (infd == -1)
    {
        printf("infd open() err...\n");
        exit(0);
    }

    int outfd;
    // 打开管道文件，准备写数据
    // outfd = open("fifop", O_WRONLY | O_NONBLOCK);
    outfd = open("fifop", O_WRONLY); // 阻塞模式
    if (outfd == -1)
    {
        printf("outfd open() err...\n");
        exit(0);
    }

    char buf[1024];
    int n;
    // 从文件中读取数据并写入管道文件
    while ((n = read(infd, buf, 1024)) > 0)
    {
        write(outfd, buf, n);
    }

    close(infd);
    close(outfd);

    printf("fifow写管道文件成功\n");
    return 0;
}