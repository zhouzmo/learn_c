#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int fd[2];
    int ret = 0;
    char write_buf[] = "Hello linux";
    char read_buf[128] = {0};
    
    /*
    函数原型: int pipe(int fd[2])
    参数: 管道文件描述符，有两个文件描述符，分别是fd[0]和fd[1]，
    管道有一个读端fd[0]和一个写端fd[1]
    返回值： 0表示成功；1表示失败
    */
    ret = pipe(fd);
    if (ret < 0)
    {
        printf("create pipe fail\n");
        return -1;
    }
    printf("create pipe success fd[0]=%d fd[1]=%d\n", fd[0], fd[1]);

    // 向文件描述符fd[1]写管道
    write(fd[1], write_buf, sizeof(write_buf));

    // 从文件描述符fd[0]读管道
    read(fd[0], read_buf, sizeof(read_buf));
    printf("read_buf=%s\n", read_buf);

    close(fd[0]);
    close(fd[1]);
    return 0;
}

/*
 gcc 1-pipe_write.c
./a.out

create pipe sucess fd[0]=3 fd[1]=4
read_buf=Hello linux

*/