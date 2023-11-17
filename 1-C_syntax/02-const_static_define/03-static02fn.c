//---fn.c
#include <stdio.h>
extern int num; //声明 num 是外部变量
void fn()
{
    num++;
    printf("fn n=%d\n",num);
}