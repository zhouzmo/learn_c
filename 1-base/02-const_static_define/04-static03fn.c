//---fn.c
#include <stdio.h>

// 声明为静态函数 -> 隐藏 隔离 保护(外部文件不能访问 改变作用域)
// 注:在C++中就比较重要，赋予对象，唯一共享的概念
// void fn()
static void fn()
{
    printf("fn go\n");
}