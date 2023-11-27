#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    int *x = (int*)malloc(sizeof(int));
    *x = 123;
    printf("Initial address of x: %p\n", x);
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // 子进程
        printf("Child process: x = %d, address = %p\n", *x, x);
        
        *x = 456; // 修改 x
        
        printf("Child process: x = %d, address = %p\n", *x, x);
    } else if (pid > 0) {
        // 父进程
        printf("Parent process: x = %d, address = %p\n", *x, x);
        
        *x = 789; // 修改 x
        
        printf("Parent process: x = %d, address = %p\n", *x, x);
    }
    
    free(x); // 释放内存
    
    return 0;
}