#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char **argv)
{

    if (argc != 2)
    {
        printf("Usage: %s <file>\n", argv[0]);
        return -1;
    }

    int fd = open(argv[1], O_RDONLY);

    printf("fd = %d\n", fd);
    
    while (1)
    {
        sleep(100);
    }
    return 0;
}

/*


$ gcc 06_app_libc_kernel.c
$ ./a.out  06_app_libc_kernel.c &
[1] 15642
 fd = 3

$ ./a.out  06_app_libc_kernel.c &
[2] 15662
 fd = 3


   PID TTY          TIME CMD
 15415 pts/8    00:00:00 bash
 15642 pts/8    00:00:00 a.out
 15662 pts/8    00:00:00 a.out
 15663 pts/8    00:00:00 ps


$ cd /proc/15642/fd
$ ll
total 0
dr-x------ 2 book book  0 Oct 25 10:28 ./
dr-xr-xr-x 9 book book  0 Oct 25 10:27 ../
lrwx------ 1 book book 64 Oct 25 10:28 0 -> /dev/pts/8
lrwx------ 1 book book 64 Oct 25 10:28 1 -> /dev/pts/8
lrwx------ 1 book book 64 Oct 25 10:28 2 -> /dev/pts/8
lr-x------ 1 book book 64 Oct 25 10:28 3 -> /home/book/Learn_C_CPP/1-c_learn/02-advance/001-file_io/06_app_libc_kernel/06_app_libc_kernel.c*

$ cd /proc/15662/fd
$ ll
total 0
dr-x------ 2 book book  0 Oct 25 10:33 ./
dr-xr-xr-x 9 book book  0 Oct 25 10:32 ../
lrwx------ 1 book book 64 Oct 25 10:33 0 -> /dev/pts/8
lrwx------ 1 book book 64 Oct 25 10:33 1 -> /dev/pts/8
lrwx------ 1 book book 64 Oct 25 10:33 2 -> /dev/pts/8
lr-x------ 1 book book 64 Oct 25 10:33 3 -> /home/book/Learn_C_CPP/1-c_learn/02-advance/001-file_io/06_app_libc_kernel/06_app_libc_kernel.c*

*/