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

    // fd = open(argv[1], O_RDWR);
    fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0777);
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

    while (1)
    {
        sleep(10);
    }

    close(fd);
    return 0;
}

/*
$ gcc create.c
$  ./a.out 1.txt &
[1] 14848
fd = 3


$ cd /proc/14319/fd
$ ll
total 0
dr-x------ 2 book book  0 Oct 25 08:32 ./
dr-xr-xr-x 9 book book  0 Oct 25 08:32 ../
lrwx------ 1 book book 64 Oct 25 08:32 0 -> /dev/pts/7
lrwx------ 1 book book 64 Oct 25 08:32 1 -> /dev/pts/7
lrwx------ 1 book book 64 Oct 25 08:32 2 -> /dev/pts/7
lrwx------ 1 book book 64 Oct 25 08:32 3 -> /home/book/Learn_C_CPP/1-c_learn/02-advance/001-file_io/02_create/1.txt*

$ ll 1.txt
-rwxrwxr-x 1 book book 0 Oct 25 08:31 1.txt*


*/