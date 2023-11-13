#include <unistd.h>
#include <stdlib.h>

int a;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s <number>\n", argv[0]);
        return -1;
    }
    a = strtol(argv[1], NULL, 0);
    printf("a's address = 0x%lx, a's value = %d\n", &a, a);
    
    while (1)
    {
        sleep(10);
    }
    return 0;
}

/*
$ gcc demo.c -o demo.out -static


$ ./demo.out 12 &
[1] 6472
$ a's address = 0x6bc3a0, a's value = 12

$ ./demo.out 123 &
[2] 6473
$ a's address = 0x6bc3a0, a's value = 123

$ ps
   PID TTY          TIME CMD
  6083 pts/0    00:00:00 bash
  6472 pts/0    00:00:00 demo.out
  6473 pts/0    00:00:00 demo.out
  6474 pts/0    00:00:00 ps
$

这 2 个程序同时在内存中运行，它们的值不一样，所以变量 a 的地址
肯定不同；
◼ 但是打印出来的变量 a 的地址却是一样的。

*/