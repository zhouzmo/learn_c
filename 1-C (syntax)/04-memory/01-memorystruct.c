#include <stdio.h>

/*
内存示意图1
https://gitee.com/MrZfk/note/blob/master/11-C%E8%AF%AD%E8%A8%80/img/c_memory.png
*/

const int g_A = 10;  // 常量区
int g_B = 20;        // 数据段
static int g_C = 30; // 数据段
static int g_D;      // BSS段
int g_E;             // BSS段
char *p1;            // BSS段

int main()
{
    int local_A;            // 栈
    int local_B;            // 栈
    static int local_C = 0; // BSS段(初值为0 等于没初始化，会放在BSS段 )
    static int local_D;     // 数据段

    char *p3 = "123456"; // 123456在代码段，p3在栈上

    p1 = (char *)malloc(10);       // 堆，分配得来得10字节的区域在堆区
    char *p2 = (char *)malloc(20); // 堆上再分配，向上生长
    strcpy(p1, "123456");          // 123456{post.content}放在常量区，编译器可能会将它与p3所指向 的"123456"优化成一块

    printf("hight address\n");

    printf("-------------栈--------------\n");
    printf("栈,    局部变量,                                   int local_A, addr:0x%08x\n", &local_A);
    printf("栈,    局部变量,(后进栈地址相对local_A低)           int local_B, addr:0x%08x\n", &local_B);

    printf("-------------堆--------------\n");
    printf("堆,    malloc分配内存,     char *p2 = (char *)malloc( 20 ),     addr:0x%08x\n", p2);
    printf("堆,    malloc分配内存,           p1 = (char *)malloc( 10 ),     addr:0x%08x\n", p1);
    printf("------------BSS段------------\n");
    printf("BSS段, 全局变量,       未初始化                int  g_E;  ,     addr:0x%08x\n", &g_E, g_E);
    printf("BSS段, 静态全局变量,   未初始化,         static int   g_D;,     addr:0x%08x\n", &g_D);
    printf("BSS段, 静态局部变量,   未初始化,   static int  g_C = 30; ,      addr:0x%08x\n", &local_C);
    printf("BSS段, 静态局部变量,   未初始化,         static int  local_D; , addr:0x%08x\n", &local_D);

    printf("-----------数据段------------\n");
    printf("数据段,全局变量,       初始化            int   g_B  = 20;,     addr:0x%08x\n", &g_B);
    printf("数据段,静态全局变量,   初始化,      static int  g_C  = 30;,     addr:0x%08x\n", &g_C);

    printf(" 常量区           只读const,      const int g_A = 10; ,     addr:0x%08x\n\n", &g_A);

    printf("-----------代码段------------\n");
    printf(" 程序代码，可反汇编看 objdump -d a.out \n");
    printf("low address\n");
    return 0;
}

/*
hight address
内核程序

-------------栈--------------
栈,    局部变量,                                   int local_A, addr:0xffffdda0
栈,    局部变量,(后进栈地址相对local_A低)           int local_B, addr:0xffffdda4
-------------堆--------------
堆,    malloc分配内存,     char *p2 = (char *)malloc( 20 ),     addr:0x555592c0
堆,    malloc分配内存,           p1 = (char *)malloc( 10 ),     addr:0x555592a0
------------BSS段------------
BSS段, 全局变量,       未初始化                int  g_E;  ,     addr:0x55558030
BSS段, 静态全局变量,   未初始化,         static int   g_D;,     addr:0x5555801c
BSS段, 静态局部变量,   未初始化,   static int  g_C = 30; ,      addr:0x55558020
BSS段, 静态局部变量,   未初始化,         static int  local_D; , addr:0x55558024
-----------数据段------------
数据段,全局变量,       初始化            int   g_B  = 20;,     addr:0x55558010
数据段,静态全局变量,   初始化,      static int  g_C  = 30;,     addr:0x55558014
 常量区           只读const,      const int g_A = 10; ,     addr:0x55556008

-----------代码段------------
 程序代码，可反汇编看 objdump -d a.out 
low address



*/