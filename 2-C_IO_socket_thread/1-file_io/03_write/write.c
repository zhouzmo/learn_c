#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

/*
ssize_t write(int fd, const void *buf, size_t count);
    fd：文件描述符，用于指定要写入的文件。
    buf：指向要写入的数据的缓冲区。
    count：要写入的字节数。

*/

int main(int argc, char **argv)
{
    int fd;
    int i;
    int len;

    if (argc < 3)
    {
        printf("Usage: %s <file> <string1> <string2> ...\n", argv[0]);
        return -1;
    }

    fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        printf("can not open file %s\n", argv[1]);
        // printf("errno = %d\n", errno);
        // printf("err: %s\n", strerror(errno));
        // perror("open");
    }
    else
    {
        printf("fd = %d\n", fd);
    }

    for (i = 2; i < argc; i++)
    {
        len = write(fd, argv[i], strlen(argv[i]));
        if (len != strlen(argv[i]))
        {
            perror("write");
            break;
        }
        write(fd, "\r\n", 2);
    }

    close(fd);
    return 0;
}

/*
$ gcc write.c
$ ./a.out 1.txt hello1 hello2
fd = 3
$ cat 1.txt
hello1
hello2


*/