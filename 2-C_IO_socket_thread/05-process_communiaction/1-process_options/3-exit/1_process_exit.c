#include <stdio.h>
#include <stdlib.h>

/*

进程终止的5种方式,
   正常退出
       从 main函数返回
       调用exit    系统调用
       调用_exit   内核调用
   异常退出·
       调用abort产生SIGABOUT信号
       由信号终止ctrl+c SlGINT


exit结束进程时会刷新缓冲区，_exit不会；
*/

void bye(void)
{
    printf("~bye bye~\n");
}

int main(int argc, char *argv[])
{
    int i;
    printf("hello world");

    /* 1. _exit 是系统调用 */
    // _exit(0);//不打印，注意：加上 \n 会打印，\n有清除缓冲作用

    /* 2. exit 是 C 库函数，会调用终止处理程序，清除I/O缓冲 */
    i = atexit(bye); // 注册 终止处理程序，函数结束会调用
    if (i != 0)
    {
        fprintf(stderr, "cannot set exit function\n");
        exit(EXIT_FAILURE);
    }
    // exit(EXIT_SUCCESS);

    /* 4. abort */
    // abort();//无打印，无回调

     /* 5. abort */
   while(1);// Ctrl + C,打断

    /* 3. 正常返回 */
    return 0;
}
