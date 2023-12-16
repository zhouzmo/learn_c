#include <stdio.h>
#include <stdlib.h>

/*
malloc申请的空间大小
在一个拥有 1GB 内存的计算机中，malloc 能否成功分配 1.2GB 的内存取决于多个因素。
首先，让我们来看看 malloc 能够申请的空间大小的限制。
在 C 语言中，malloc 函数用于动态分配内存。
其参数是 size_t 类型，这是一个无符号整数类型，通常被用来表示内存大小。
在大多数系统中，size_t 的大小通常是 32 位或 64 位，取决于操作系统和编译器的架构。
对于 32 位系统，size_t 的最大值是 4,294,967,295，即 0xFFFFFFFF。
对于 64 位系统，size_t 的最大值是 18,446,744,073,709,551,615，即 0xFFFFFFFFFFFFFFFF。
因此，malloc 能够申请的空间大小受到 size_t 类型的限制。

*/
int main() {
    char *p;
    const unsigned int k = 1024 * 1024 * 1024 * 1.2; // 1.2GB in bytes
    printf("%x \n", k);
    p = (char *)malloc(k);
    if (p != NULL)
        printf("OK");
    else
        printf("error");
    return 0;
}