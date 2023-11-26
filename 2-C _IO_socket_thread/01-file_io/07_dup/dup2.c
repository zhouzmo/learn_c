#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>




int main(int argc, char **argv)
{
    int fd;
    
    if (argc != 2)
    {
        printf("Usage: %s <file>\n", argv[0]);
        return -1;
    }

    fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0777);
    if (fd < 0)
    {
        printf("can not open file %s\n", argv[1]);
        printf("errno = %d\n", errno);
        printf("err: %s\n", strerror(errno));
        perror("open");
    }
    else
    {
        printf("fd = %d\n", fd);
    }
    /*
    `dup2` 函数用于复制已有的文件描述符，并将新的文件描述符指定为参数。
    如果新的文件描述符已经打开，它将首先被关闭
    ，然后 `dup2` 将原始文件描述符复制到新的文件描述符，允许您控制新文件描述符的值。
    */
    dup2(fd, 1); //fd为老文件描述符 ，1为标准输出

    printf("hello, world\n");  /* 打印到fd=1的文件  fd=1在shell里面是标准输出  */
    return 0;
}


/*
$ cat a.txt
123

$ gcc dup2.c
$ ./a.out a.txt
fd = 3

$ cat a.txt
hello, world



*/