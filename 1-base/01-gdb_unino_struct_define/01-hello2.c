#include <stdio.h>

/*
    $ gcc main.c
    $ ./a.out hello  world
    argc: 命令行参数个数
    argv: 用指针数组存储 参数， 第一个 是执行文件的名字（a.out)
*/

int main(int argc, char *argv[])
{
    int i;
    printf("argc = %d\n",argc);
    printf("argv[] = %s ",argv[0]);
    for (i = 1; i < argc; i++)
    {
        printf("%s ", argv[i]);
    }
    printf("\n");
    return 0;
}