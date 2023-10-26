#include <stdio.h>

/*
https://blog.csdn.net/CharlesJhonson/article/details/104862665

结构体占用的内存大于等于所有成员占用的内存的总和（成员之间可能会存在缝隙），
共用体占用的内存等于最长的成员占用的内存。共用体使用了内存覆盖技术，
同一时刻只能保存一个成员的值，如果对新的成员赋值，就会把原来成员的值覆盖掉。

联合体的内存除了取最大成员内存外，还要保证是所有成员类型size的最小公倍数

注： union 整体占用空间必须为 其他类型的整数被


*/

struct Test001
{
    int tint;
    char tchar;
    short tshort;
};
/*
    内存结构
    |   |   |   |   |
    |    int        |
    char|---|short  |

    4 + 2 + 2 = 8
*/

struct Test002
{
    char tchar;
    int tint;
    short tshort;
};
/*
    内存结构
    |   |   |   |   |
    |char-----------|
    |    int        |
    |short  |------ |

    4 + 4 + 4 = 12
*/

union unionDataInt
{
    char str[3];
    // long long num;
    int num;
    char c;
};

union unionDataLong
{
    char str[3];
    long long num;
    //  int num;
    char c;
};

int main()
{
    union unionDataInt udInt;
    union unionDataLong udLong;

    struct Test001 structTest1;
    struct Test002 structTest2;

    printf("sizeof(structTest1):%d\t,sizeof(structTest2):%d\t\n", sizeof(structTest1), sizeof(structTest2));
    // sizeof(structTest1):8	,sizeof(structTest2):12

    printf("sizeof(udInt.str):%d\t, sizeof(udInt.num):%d\t, sizeof(udLong.num):%d\t,sizeof(udInt.c):%d\n",
           sizeof(udInt.str), sizeof(udInt.num), sizeof(udLong.num), sizeof(udInt.c));
    // sizeof(udInt.str):3	, sizeof(udInt.num):4	, sizeof(udLong.num):8	,sizeof(udInt.c):1
    
    printf("sizeof(udInt):%d\t,sizeof(udLong):%d\t\n", sizeof(udInt), sizeof(udLong));
    // sizeof(udInt):4	,sizeof(udLong):8
/*
    union 整体占用空间必须为 其他类型的整数倍
    udInt 3 4 1    ->  4
    udLong 3 8 1   ->  8
*/

    

    return 0;
}
