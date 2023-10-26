/**********************************************************************
 * 功能描述： 1.创建一个有名管道
 ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    int ret;

    ret = mkfifo("./myfifo", 0777);   //创建有名管道，文件权限为777
    if (ret < 0) {
        printf("create myfifo fail\n");
        return -1;
    }
    printf("create myfifo sucess\n");
    
    return 0;
}

/*
$ gcc 1-create_fifo.c
$ ./a.out
create myfifo sucess
$ ll
总计 40
drwxr-xr-x 2 lu lu  4096 10月 26 09:55 ./
drwxr-xr-x 5 lu lu  4096 10月 25 17:01 ../
-rwxr--r-- 1 lu lu   570 10月 26 09:49 1-create_fifo.c*
-rwxrwxr-x 1 lu lu 16008 10月 26 09:55 a.out*
prwxrwxr-x 1 lu lu     0 10月 26 09:55 myfifo|
# p文件描述符，管道
*/