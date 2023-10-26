
#include <stdio.h>
#include <string.h>

/*
================================================================
! void *memset(void *ptr, int value, size_t num);
将指定内存块的连续字节设置为指定的值，以便初始化、清零或填充内存。这个函数非常有用，可以用于初始化数组、字符串缓冲区等。


参数：
ptr：是一个指向要设置的内存块的指针，通常是一个数组或缓冲区的首地址。
value：是要设置的值，通常是一个整数。该值将被转换为 unsigned char 类型并设置到内存块中。
num：是要设置的连续字节数，表示要设置的内存块的大小。
返回值：
memset 函数没有返回值（返回类型是 void*），它会直接修改传入的 ptr 指向的内存块。
================================================================

*/

void memsetTest()
{
     char str[50];

    // 使用 memset 将字符数组 str 中的内容设置为 'A'
    memset(str, 'A', sizeof(str));

    // 打印设置后的字符串
    printf("设置后的字符串: %s\n", str);

    return 0;
}

int main()
{
     memsetTest();
}