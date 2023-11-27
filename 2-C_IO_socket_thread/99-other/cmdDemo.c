#include <stdio.h>
#include <string.h>

/*
argc 表示命令行参数的数量，包括程序名称本身。
argv 是一个指向字符串数组的指针，其中存储了命令行参数的值。
*/
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("No command-line arguments provided.\n");
    }
    else
    {
        printf("Number of command-line arguments: %d\n", argc - 1); // Subtract 1 for the program name

        for (int i = 0; i < argc; i++)
        {
            printf("Argument %d: %s\n", i, argv[i]);
        }
    }

    return 0;
}
/*
 gcc .\cmdDemo.c -o cmdDemo.exe
 .\cmdDemo.exe arg1 arg2 arg3


Number of command-line arguments: 3
Argument 0: F:\Learn_C_CPP\1-c_learn\02-advance\001-cmdCode\cmdDemo.exe
Argument 1: arg1
Argument 2: arg2
Argument 3: arg3

*/