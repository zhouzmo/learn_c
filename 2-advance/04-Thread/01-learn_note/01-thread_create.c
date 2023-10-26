#include <stdio.h>
#include <pthread.h>




 /*
int pthread_create(pthread_t *th, const pthread_attr_t *attr, 
                void ( func)(void *), void *arg);
该函数用于创建一个新的线程，允许并行执行指定的函数。
参数：
th: 
    一个指向 pthread_t 类型的指针，用于存储新创建线程的标识符。
attr: 
    一个指向 pthread_attr_t 类型的指针，用于指定线程的属性，
    通常可以设置为 NULL，表示使用默认属性。
func:
    一个指向函数的指针，该函数将作为新线程的入口点执行。
    该函数接受一个 void* 类型的参数，并返回一个 void* 类型的指针。
arg: 
    一个 void* 类型的参数，传递给 func 函数作为其参数。
返回值：
    如果函数调用成功，它将返回 0，表示线程创建成功。
    如果失败，它将返回一个非零的错误码，用于指示错误的原因。

*/



void *fun1(void *arg)
{
    printf("fun1 thread tid = %lu\n", 
   (unsigned long)pthread_self());
   
}


int main(void)
{
    //  获取主线程的 tid
    pthread_t tid = pthread_self();
    printf("tid = %lu\n", (unsigned long)tid);

    // 创建线程
    pthread_t tid1;
    int ret = pthread_create(&tid1, NULL, fun1, NULL); // 创建线程
    // 创建线程失败使用
    if (ret != 0)
    {
        perror("pthread_create");
        return -1;
    }
    printf("tid_main = %lu tid_new = %lu \n", (unsigned long)pthread_self(), (unsigned long)tid1);

   // 等待 2 线程的执行
    sleep(2);
    return 0;
}
/*
tid = 1
tid_main = 1 tid_new = 2
fun1 thread tid = 2

*/