#include <stdio.h>

int main(int argc, char const *argv[])
{

    /*
        char a[] = "abcd";
        gdb调试：    x &a
        0x61fe1b:       0x64636261
        0x 64 63 62 61
        61转为10进制为97，对应的ascll码是 'a',同理，前面为d c b a
        当修改地址 0x61fe1b + 1 ,内存中就会指向 b，能修改已经分配的空间
    */
    //   char a[] = "abcd"; //定义字符串数组，并分配空间(有包含结尾字符'\0')

    /*
        char *a =  "abcd";
        gdb调试  x &a
        0x61fe18:       0x00404000
        0x00404000 指向字符串常量，不可以修改
        *(a+1) =3
        0x00404000 + 1 该地址还是字符串常量的区域，不可以修改
    */
    char *a = "abcd"; // 定义指向字符串常量的指针，但不分配空间(常量)
    // *(a+1) =3  段错误(不能修改字符串常量的内容，因未分配空间)
    int *num = {1,2,3,4};

    printf("%c  %c\n", a[2], *(a + 2)); // a[i] 与 *(a+i) 等价
    printf("%x  %x\n", &a[2], a + 2);   //&a[i] 与 a+2    等价
    printf("%d\n", sizeof(a));          // 求字符串长度(包含结尾字符'\0')
    printf("%d\n", sizeof(a[1]));       // 求字符串长度(包含结尾字符'\0')
    printf("%d\n", a[-1]);              // 数组的第一个元素之前的元素(可能是未知的值)

    printf("%c  %c\n", num[2], *(num + 2)); // a[i] 与 *(a+i) 等价
    printf("%x  %x\n", &num[2], num + 2);   //&a[i] 与 a+2    等价
    printf("%d\n", sizeof(a));          // 求字符串长度(包含结尾字符'\0')
    printf("%d\n", sizeof(a[1]));       // 求字符串长度(包含结尾字符'\0')
    printf("%d\n", a[-1]);              // 数组的第一个元素之前的元素(可能是未知的值)
}
