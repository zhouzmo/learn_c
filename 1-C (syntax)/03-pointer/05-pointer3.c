#include <stdio.h>

int add10(int num)
{
    return num + 10;
}


int (*pfun)(int);

int (*a[10])(int);

int main(int argc, char const *argv[])
{

    printf("函数指针--------------------------------------\n");
    pfun = add10;
    printf("%d\n", pfun(5));
   

    printf("指针试题--------------------------------------\n");
    char str1[] = "abc";
    char str2[] = "abc";
    printf("%d\n", str1 == str2); // 不等: 因 字符串数组 会分配独立的空间

    char *str3 = "abc";
    char *str4 = "abc";
    printf("%d\n", str1 == str2); // 相等: 因 指向 相同的字符串常量区域

    printf("%d\n", sizeof(str1)); // sizeof 要包含结尾字符\0
    printf("%d\n", strlen(str1)); // strlen 遇\0 结束， \0不计入长度
    printf("%d\n", sizeof(str3)); // 指针的大小
    printf("%d\n", strlen(str4)); // 动态计算指向的字符串 的大小  \0不计入长度



     return 0;
}

