#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
/*
`mkfifo` 函数用于创建命名管道（FIFO）。下面是 `mkfifo` 函数的文档说明：


int mkfifo(const char *pathname, mode_t mode);

**参数：**

- `pathname`：要创建的命名管道的路径名。
- `mode`：设置文件权限的八进制数值。通常使用 `0644` 来设置权限为读写权限。

**返回值：**

- 如果成功创建了命名管道，则返回0。
- 如果发生错误，则返回-1，并设置 `errno` 来指示错误的原因。

**功能：**

`mkfifo` 函数创建一个命名管道，该管道可以用于进程间的通信。命名管道是一种特殊类型的文件，它提供了一个先进先出（FIFO）的数据流。一个进程可以将数据写入管道的一端，而另一个进程可以从管道的另一端读取数据。

**注意事项：**

- `mkfifo` 函数创建的命名管道是阻塞的，即当没有数据可读时，读取操作会被阻塞，直到有数据可读。
- `mkfifo` 函数创建的命名管道在文件系统中以文件的形式存在，可以使用文件系统相关的操作函数对其进行读写操作。
- 在使用命名管道进行进程间通信时，需要注意管道的读写顺序，以免产生死锁或数据丢失的问题。


*/
int main(int argc, char *argu[])
{
    // 打开2.txt准备写数据
    int outfd;
    outfd = open("./2.txt", O_RDWR | O_CREAT | O_TRUNC, 0777);
    // outfd = open("./2.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (outfd == -1)
    {
        printf("open() 2.txt err...\n");
        exit(0);
    }

    // 打开管道文件，准备读数据
    int infd;
    infd = open("fifop", O_RDONLY);
    if (infd == -1)
    {
        printf("open() fifop err...\n");
        exit(0);
    }

    char buf[1024];
    int n;
    // 从管道文件中读取数据并写入2.txt
    while ((n = read(infd, buf, 1024)) > 0)
    {
        write(outfd, buf, n);
    }

    close(infd);
    close(outfd);
    unlink("fifop");

    printf("读管道文件成功\n");
    return 0;
}