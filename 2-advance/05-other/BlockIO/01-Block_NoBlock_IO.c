#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main() {
    int fd;
    char buf;

    // 打开一个文件（可以是任何文本文件）
    // fd = open("sample.txt", O_RDONLY | O_NONBLOCK);
    fd = open("sample.txt", O_RDONLY );
    if (fd == -1) {
        perror("Unable to open the file");
        return -1;
    }

    // 非阻塞读取字符
    while (1) {
        ssize_t res = read(fd, &buf, 1);
        if (res > 0) {
            printf("Read character: %c\n", buf);
        } else if (res == 0) {
            // 没有数据可读
            usleep(100000); // 等待一段时间后再尝试读取
        } else if (res == -1) {
            if (errno == EAGAIN) {
                // 非阻塞文件没有数据可读
                usleep(100000);
            } else {
                // 出现错误
                perror("Error reading from the file");
                break;
            }
        }
    }

    close(fd);
    return 0;
}
/*
 gcc -o blockio.out blockio.c
book@100ask:~/workSpace/code$ ./blockio.out


重新开一个链接
 echo "handsa"  >> sample.txt
book@100ask:~/workSpace/code$ echo "handsa"  >> sample.txt
book@100ask:~/workSpace/code$ echo "handsa"  >> sample.txt
book@100ask:~/workSpace/code$ echo "handsa"  >> sample.txt

*/