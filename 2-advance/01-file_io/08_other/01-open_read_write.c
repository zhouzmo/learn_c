#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>


/*
系统函数

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
================================================================
ssize_t read(int fd, void *buf, size_t count);
    fd：文件描述符，用于指定要读取的文件。
    buf：指向要读取数据存储的缓冲区。
    count：要读取的字节数。
================================================================
ssize_t write(int fd, const void *buf, size_t count);
    fd：文件描述符，用于指定要写入的文件。
    buf：指向要写入的数据的缓冲区。
    count：要写入的字节数。
*/


#define LENGTH 100
main()
{
    int fd, len;
    char buf[LENGTH];
    char *filePath = "hello.txt";
    char *strContent = "hello,world";

    fd = open(filePath, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

    // 写文件
    if (fd)
    {
        write(fd, strContent, strlen(strContent));
        close(fd);
    }

    // 读文件
    fd = open(filePath, O_RDWR);
    len = read(fd, buf, LENGTH);
    buf[len] = "\0";
    printf("strContent: %s\n", buf);
    close(fd);
}
