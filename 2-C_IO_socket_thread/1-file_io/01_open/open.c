#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

/*
int open(const char *pathname, int flags, mode_t mode);
    pathname：指向要打开的文件的路径字符串。
    flags：打开文件的标志，用于指定文件的打开方式和操作选项。常见的标志包括：
            O_RDONLY：只读方式打开文件。
            O_WRONLY：只写方式打开文件。
            O_RDWR：可读可写方式打开文件。
            O_CREAT：如果文件不存在，则创建文件。
            O_APPEND：追加写入文件。
            等等。可以通过使用位运算符组合多个标志。
    mode：当使用O_CREAT标志创建新文件时，用于指定文件的访问权限。
            这个参数只有在创建文件时才有效，可以使用文件访问权限的八进制表示。
            例如，0644表示所有者具有读写权限，其他用户具有只读权限。


*/

int main(int argc, char **argv)
{
    int fd;

    if (argc != 2)
    {
        printf("Usage: %s <file>\n", argv[0]);
        return -1;
    }

    fd = open(argv[1], O_RDWR);
    if (fd < 0)
    {
        printf("can not open file %s\n", argv[1]);
        // printf("errno = %d\n", errno);
        // printf("err: %s\n", strerror(errno));
        // perror("open");
    }
    else
    {
        printf("fd = %d\n", fd);
    }

    while (1)
    {
        sleep(10);
    }

    close(fd);
    return 0;
}
