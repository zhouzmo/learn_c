#include <stdio.h>

/*

结构体占用的内存大于等于所有成员占用的内存的总和（成员之间可能会存在缝隙），
共用体占用的内存等于最长的成员占用的内存。共用体使用了内存覆盖技术，
同一时刻只能保存一个成员的值，如果对新的成员赋值，就会把原来成员的值覆盖掉。'

注： union 整体占用空间必须为 其他类型的整数被


*/
union data
{
    char str[9];
    long long n;
    char ch;
};

int main()
{
    union data a;
    printf("long:%d,int:%d\n", sizeof(long long), sizeof(int)); // 8 4
    for (int i = 0; i < sizeof(a); i++)
    {
        a.str[i] = 1;
    }
    printf("str数字初始值");
    for (int i = 0; i < sizeof(a); i++)
    {
        printf("%d", a.str[i]);
    }
    printf("\n");

    // 结构体变量在内存中存储时需要满足对齐原则。
    printf("%d, %d,%d\n", sizeof(a.str), sizeof(a), sizeof(union data)); // 9 16 16

    a.n = 0x33;
    printf("%d, %d\n", a.n, a.ch);

    for (int i = 0; i < sizeof(a); i++)
    {
        printf("%x", a.str[i]);
    }

    return 0;
}
