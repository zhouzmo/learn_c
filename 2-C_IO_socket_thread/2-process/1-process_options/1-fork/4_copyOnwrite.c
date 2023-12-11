#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/*

写时复制copy on write
	如果多个进程要读取它们自己的那部分资源的副本，那么复制是不必要的。
	每个进程只要保存一个指向这个资源的指针就可以了。
	如果一个进程要修改自己的那份资源的“副本”，那么就会复制那份资源。这就是写时复制的含义
原因分析：
    1.加快速度，linux内核是段页式管理机制（因段管理从0开始，），也可叫页式管理机制。
    2.子进程只复制需要的页，不会完全复制所有的页。需要其他页内存时，引发缺页异常，再从父进程拷贝过来相应的其他页。
    3.在子进程没有产生缺页异常之前，在父进程中要修改一些值，内核也会将更新之前的值拷贝到子进程的内存页中。因为子进程没有想修改这些值。
页是物理内存的单位。
*/
int main()
{
    int *x = (int *)malloc(sizeof(int));
    *x = 123;
    printf("Initial address of x: %p\n", x);

    pid_t pid = fork();

    if (pid == 0)
    {
        // 子进程
        printf("Child process: x = %d, address = %p\n", *x, x);

        *x = 456; // 修改 x

        printf("Child process: x = %d, address = %p\n", *x, x);
    }
    else if (pid > 0)
    {
        // 父进程
        printf("Parent process: x = %d, address = %p\n", *x, x);

        *x = 789; // 修改 x

        printf("Parent process: x = %d, address = %p\n", *x, x);
    }

    free(x); // 释放内存

    return 0;
}
