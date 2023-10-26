/**********************************************************************
 * 功能描述： 1.只读方式打开这个有名管道文件，并读取这个值
              2.当这个值非零时，继续执行后面的打印输出语句
 ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int i;
    int fd=open("./3rd_fifo", O_RDONLY);
    char p_flag = 0;
    
    if (fd < 0) {
        printf("open 3rd_fifo fail\n");
        return -1;
    }
    
    printf("open 3rd_fifo sucess\n");
    read(fd, &p_flag, sizeof(p_flag));

    while(!p_flag); // 等待 p_flag 改变

    for (i = 0; i < 5; i++) {
        printf("this is second process i=%d\n", i);
        usleep(100);
    }

    while(1);
    return 0;
}
/*
$ gcc -o 2-named_pipe.out 2-named_pipe.c
$ gcc -o 2-named_pipe_2.out 2-named_pipe_2.c


$ ./2-named_pipe.out &
[1] 138254
create named pipe sucess

$ ./2-named_pipe_2.out
open 3rd_fifo sucess
open 3rd_fifo sucess
this is first process i=0
this is first process i=1
this is first process i=2
this is first process i=3
this is first process i=4
this is second process i=0
this is second process i=1
this is second process i=2
this is second process i=3
this is second process i=4



*/