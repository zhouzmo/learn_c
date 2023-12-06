#include <stdio.h> // 包含标准输入输出函数库
#include <string.h> // 包含字符串处理函数库

int main()
{
    FILE *fp = 0; // 定义文件指针变量fp

    char strbuf[301]; // 存放从文件中读取到的一行的内容

    // 以只读的方式打开文件/oracle/c/book1.c
    if ((fp = fopen("/oracle/c/book1.c", "rt")) == 0)
    {
        printf("fopen(/oracle/c/book1.c) failed.\n"); // 打开文件失败时打印错误信息
        return -1; // 返回错误代码-1
    }

    fseek(fp, -200, 2); // 将文件指针移动到文件末尾前200个字节处

    printf("%d\n", ftell(fp)); // 打印当前文件指针的位置

    // 逐行读取文件的内容，输出到屏幕
    while (1)
    {
        memset(strbuf, 0, sizeof(strbuf)); // 清空strbuf缓冲区
        if (fgets(strbuf, 301, fp) == 0) // 从文件中读取一行到strbuf中，如果读取失败则退出循环
            break;
        printf("%d : %s", ftell(fp), strbuf); // 打印当前文件指针的位置和读取到的内容
    }

    // 关闭文件
    fclose(fp);

    return 0; // 返回成功代码0
}