#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#define PTHREAD_CATCH(ret)        \
    if (ret != 0)                 \
    {                             \
        perror("pthread_error"); \
        return -1;                \
    }


int Num = 0;           // 公共临界变量
pthread_mutex_t mutex; // 互斥量变量 一般申请全局变量

void *fun1(void *arg)
{
    pthread_mutex_lock(&mutex); // 加锁 若有线程获得锁，则会阻塞
    while (Num < 3)
    {
        Num++;
        printf("%s:Num = %d\n", __FUNCTION__, Num);
        sleep(1);
    }
    pthread_mutex_unlock(&mutex); // 解锁
    pthread_exit(NULL);
}

void *fun2(void *arg)
{
    pthread_mutex_lock(&mutex); // 加锁 若有线程获得锁，则会阻塞
    while (Num > -3)
    {
        Num--;
        printf("%s:Num = %d\n", __FUNCTION__, Num);
        sleep(1);
    }
    pthread_mutex_unlock(&mutex); // 解锁
    pthread_exit(NULL);
}

int main()
{
    int ret;
    pthread_t tid1, tid2;
    ret = pthread_mutex_init(&mutex, NULL); // 初始化互斥量
    // PTHREAD_CATCH(ret);
    ret = pthread_create(&tid1, NULL, fun1, NULL); // 创建线程 1
    // PTHREAD_CATCH(ret);
    ret = pthread_create(&tid2, NULL, fun2, NULL); // 创建线程 2
    // PTHREAD_CATCH(ret) ;
    pthread_join(tid1, NULL);      // 阻塞回收线程 1
    pthread_join(tid2, NULL);      // 阻塞回收线程 2
    pthread_mutex_destroy(&mutex); // 销毁互斥量
    return 0;
}

