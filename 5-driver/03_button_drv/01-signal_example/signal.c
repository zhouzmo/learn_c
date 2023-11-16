#include <stdio.h>
#include <unistd.h>
#include <signal.h>
void my_sig_func(int signo)
{
    printf("get a signal : %d\n", signo);
}

int main(int argc, char **argv)
{
    int i = 0;
    
    // 注册信号处理函数
    // signal(SIGIO, my_sig_func);
    
    while (1) 
    {
        printf("Hello, world %d!\n", i++);
        sleep(2);
    }
    
    return 0;
}


/*

$ gcc signal.c
$ ls
a.out  signal.c
$ ls
a.out  signal.c
$ ./a.out 

Hello, world 0!
。。。。 
Hello, world 52!
get a signal : 29
Hello, world 53!

# 重新打开一个窗口
$ ps  -A |grep a.out
 48006 pts/5    00:00:00 a.out
$ kill -SIGIO 48006
# 关闭
$ kill -9 48006
*/



/*
# 不注册信号处理函数时，程序出现 I/O possible
$ kill -SIGIO 48209


$ ./a.out 
Hello, world 12!
Hello, world 13!
I/O possible

$ ps  -A |grep a.out
*/

