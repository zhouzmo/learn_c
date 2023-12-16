#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/*
malloc能够申请的空间到底能达到多大，还真是一个比较复杂的问题。
想知道在一台机器上malloc能够申请的最大空间到底是多少，
可以使用下面的程序进行测试。


您发现在当前的 Windows 环境中，malloc 能够成功分配超过 1.9GB 的内存空间。
这个结果是符合预期的，因为在 64 位 Windows 系统上，
malloc 可以成功分配远远超过 1.9GB 的内存空间。

需要注意的是，您所得到的结果是特定环境下的测试结果，
实际的最大可分配内存空间受到多种因素的影响，
包括操作系统版本、系统配置、程序本身的大小和复杂度、
动态链接库的数量和大小、以及其他正在运行的进程等因素。


*/
unsigned maximum = 1024 * 1024 * 1024;

int main(int argc, char *argv[])
{
    unsigned blocksize[] = {1024 * 1024, 1024, 1};
    int i, count;
    void *block;
    int len = sizeof(blocksize) / sizeof(unsigned);
    printf("sizeof(blocksize) / sizeof(unsigned) %d \n", len);

    for (i = 0; i < sizeof(blocksize) / sizeof(unsigned); i++)
    {
        for (count = 1;; count++)
        {
            block = malloc(maximum + blocksize[i] * count);
            if (block != NULL)
            {
                maximum = maximum + blocksize[i] * count;
                free(block);
            }
            else
            {
                break;
            }
            printf("maximum malloc size = %u bytes\n", maximum);
        }
    }
    printf("maximum malloc size = %u bytes\n", maximum);
    return 0;
}