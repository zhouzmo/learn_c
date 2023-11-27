#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char **argv)
{
    char buf[10];
    char buf2[10];
    
    if (argc != 2)
    {
        printf("Usage: %s <file>\n", argv[0]);
        return -1;
    }

    int fd1 = open(argv[1], O_RDONLY);
    int fd2 = open(argv[1], O_RDONLY);
    /*
    `dup` 函数用于复制已有的文件描述符并返回一个新的文件描述符，
    新的文件描述符指向与原始文件描述符相同的文件。通
    常用于在不关闭原始文件描述符的情况下，创建一个新的文件描述符副本。
    */
    int fd1_dup = dup(fd1);

    printf("fd1 = %d\n", fd1);
    printf("fd2 = %d\n", fd2);
    printf("fd1_dup = %d\n", fd1_dup);

    if (fd1 < 0 || fd2 < 0 || fd1_dup < 0)
    {
        printf("can not open %s\n", argv[1]);
        return -1;
    }

    read(fd1, buf, 1);       // h
    read(fd2, buf2, 1);     // h

    printf("data get from fd1 : %c\n", buf[0]);
    printf("data get from fd2: %c\n", buf2[0]);
    
    // 说明 fd1_dup 只是 fd1 的副本，对应一个相同的结构
    read(fd1_dup, buf, 1);  // e 
    printf("data get from fd1_dup: %c\n", buf[0]);
    
    return 0;
}

/*
$ gcc dup.c
$ echo hello > a.txt
$ cat a.txt
hello

# 复制 fd1 的 f_ops(位置指针)
$ ./a.out a.txt
fd1 = 3
fd2 = 4
fd1_dup = 5
data get from fd1 : h
data get from fd2: h
data get from fd1_dup: e


$


*/