#include <stdio.h>
#include <string.h>

int bb(int bbb)
{
    int *p = 0; // 定义整型指针变量p，并初始化为0
    *p = bbb;   // 将bbb的值赋给p指向的地址（错误：空指针*p赋值）
}

int aa(int aaa)
{
    bb(aaa);
}

int main()
{
    aa(13);
    int i = 1;
    printf("=%d=\n", (++i) + (++i));

    return 0;
}


/*
ulimit -a
ulimit -c unlimited 
sudo sysctl -w kernel.core_pattern=core

./a.out
 gdb ./a.out core


warning: Source file is more recent than executable.
7           *p = bbb;   // 将bbb的值赋给p指向的地址（错误：空指针*p赋值）

*/