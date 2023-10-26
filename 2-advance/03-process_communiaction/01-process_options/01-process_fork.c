#include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <sys/types.h>
 
 int main(int argc, char *argv[])
 {
     pid_t pid;
 
     pid = fork();  // 创建子进程
 
     if (pid == 0) {  // 子进程
         int i = 0;
         for (i = 0; i < 5; i++) {
             usleep(100);
             printf("PID:%d\tthis is child process i=%d\n",getpid(), i);
         }
     }
     
     if (pid > 0) {  // 父进程
         int i = 0;
         for (i = 0; i < 5; i++) {
             usleep(100);
             printf("PID:%d\tthis is parent process i=%d\n",getpid(), i);
         }
     }
 
     while(1);   //不让进程结束，以便我们查看进程的一些状态信息
     return 0;
 }