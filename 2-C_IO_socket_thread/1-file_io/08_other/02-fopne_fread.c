
#include <stdio.h>

/*
库函数
FILE *fopen(const char *filename, const char *mode);
    filename：指向要打开的文件的路径字符串。
    mode：表示文件打开模式的字符串。常用的模式包括：
        "r"：只读方式打开文件。
        "w"：写入方式打开文件，如果文件不存在则创建新文件，如果文件存在则清空文件内容。
        "a"：追加写入方式打开文件，如果文件不存在则创建新文件。
        "r+"：读写方式打开文件。
        "w+"：读写方式打开文件。如果文件不存在则创建新文件，如果文件存在则清空文件内容。

*/

#define LENGTH 100
main()
{
    FILE *fd;
    char buf[LENGTH];

    fd = fopen("fhello.txt", "w+");
    if (fd)
    {
        fputs("hello world zmo", fd);
        fclose(fd);
    }

    fd = fopen("fhello.txt", "r");
    fgets(buf, LENGTH, fd);
    printf("%s\n", buf);
    fclose(fd);
}
