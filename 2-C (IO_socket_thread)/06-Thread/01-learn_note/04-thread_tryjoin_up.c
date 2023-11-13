#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#define PTHREAD_CATCH(ret)        \
    if (ret != 0)                 \
    {                             \
        perror("pthread_create"); \
        return -1;                \
    }
/*
线程的退出情况有三种：
    第一种是进程结束，进程中所有的线程也会随之结束。
    第二种是通过函数 pthread_exit 来主动的退出线程。
    第三种被其他线程调用 pthread_cancel 来被动退出。

当线程结束后，主线程可以通过函数 pthread_join/pthread_tryjoin_np
来回收线程的资源，并且获得线程结束后需要返回的数据

1、线程主动退出
    void pthread_exit(void *retval);
    retval 需要传出的数据

2、线程被动退出
    int pthread_cancel(pthread_t thread);
    传入一个 tid 号，会强制退出该 tid 所指向的线程，若成功执行会返回 0

3、线程资源回收(阻塞方式)
    int pthread_join(pthread_t thread, void **retval);
    线程回收函数，默认状态为阻塞状态，直到成功回收线程后才返回。
    thread  要回收线程的 tid 号，
    retval  线程回收后接受线程传出的数据。

4、线程资源回收(非阻塞方式)
    int pthread_tryjoin_np(pthread_t thread, void **retval);

*/

void doTimer(int second)
{
    for (int i = 1; i <= second; i++)
    {
        sleep(1);
        printf("%s计时：%d\n", __FUNCTION__, i);
    }
}

void timer(int second)
{
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, doTimer, second);
    PTHREAD_CATCH(ret);
}

#define Num 10

void *fun(void *arg)
{
    printf("%s:start...\ttid=%lu\t,arg =%d\n",
     __FUNCTION__,  (unsigned long)pthread_self(),(int)(long)arg);
 
    sleep(3);
    // pthread_exit(arg);
}

int main(void)
{
    int ret, i, flag = 0;
    void *Tmp = NULL;
    pthread_t tid[Num];
    timer(10);
    // 创建 Num 个线程
    for (i = 0; i < Num; i++)
    {
        // printf("%s:create...,thread:%d,arg:%d\n", __FUNCTION__, i, (int)(long)i + 10);
        ret = pthread_create(&tid[i], NULL, fun, (void *)(long)(i + 10));
        PTHREAD_CATCH(ret);
    }

    // 非阻塞回收线程
    while (1)
    {
        for (i = 0; i < Num; i++)
        {
            // 判断是否回收完成
            if (_pthread_tryjoin(tid[i], &Tmp) == 0)
            {
                printf("%s:end ...,%d\n", __FUNCTION__, (int)(long)Tmp);
                flag++;
            }
        }

        if (flag >= Num)
            break;
    }

    return 0;
}

/*
fun:start...,0
fun:start...,2
fun:start...,1
main:end ...,0
fun:start...,3
fun:start...,4
main:end ...,1
main:end ...,2
main:end ...,3
main:end ...,4
*/
