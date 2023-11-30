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
    int i;
    int len;
    
    if (argc != 2)
    {
        printf("Usage: %s <file>\n", argv[0]);
        return -1;
    }

    fd = open(argv[1], O_RDWR | O_CREAT, 0644);
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

    printf("lseek to offset 3 from file head\n");
    lseek(fd, 3, SEEK_SET);

    write(fd, "123", 3);

    close(fd);
    return 0;
}

/*
$ gcc write_in_pos.c
$  ./a.out 1.txt
fd = 3
lseek to offset 3 from file head
$ cat 1.txt
hel123
hello2
*/