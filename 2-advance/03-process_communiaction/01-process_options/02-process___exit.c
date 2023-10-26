 #include <stdio.h>
 #include <stdlib.h>
 

 /*
 exit结束进程时会刷新缓冲区，_exit不会；
 */
 int main(int argc, char *argv[])
 {
     printf("hello world\n");
     printf("will exit");
     _Exit(0);    //使用_exit退出
 }