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

/*
引入：临界资源互斥问题。

int pthread_mutex_init(pthread_mutex_t *m, const pthread_mutexattr_t *a);
m   互斥量指针
a   控制互斥量的属性，一般为 NULL
函数成功后会返回 0，代表初始化互斥量成功

互斥量加锁/解锁（阻塞）
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
成功：返回 0
斥量加锁（非阻塞）
int pthread_mutex_trylock(pthread_mutex_t *mutex);

互斥量销毁
int pthread_mutex_destory(pthread_mutex_t *mutex);
传入互斥量的指针，就可以完成互斥量的销毁，成功返回 0。
*/



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
    PTHREAD_CATCH(ret);
    ret = pthread_create(&tid1, NULL, fun1, NULL); // 创建线程 1
    PTHREAD_CATCH(ret);
    ret = pthread_create(&tid2, NULL, fun2, NULL); // 创建线程 2
    PTHREAD_CATCH(ret) ;
    pthread_join(tid1, NULL);      // 阻塞回收线程 1
    pthread_join(tid2, NULL);      // 阻塞回收线程 2
    pthread_mutex_destroy(&mutex); // 销毁互斥量
    return 0;
}


/*
fun2:Num = -1
fun2:Num = -2
fun2:Num = -3
fun1:Num = -2
fun1:Num = -1
fun1:Num = 0
fun1:Num = 1
fun1:Num = 2
fun1:Num = 3
*/