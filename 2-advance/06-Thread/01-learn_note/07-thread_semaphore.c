#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>

#define PTHREAD_CATCH(ret)       \
    if (ret != 0)                \
    {                            \
        perror("pthread_error"); \
        return -1;               \
    }

/*
引入：执行的顺序又不会受到控制

初始化一个信号量
    int sem_init(sem_t *sem,int pshared,unsigned int value);成功：返回 0
        sem     sem_t 类型指针
        pshared 传入 0 代表线程控制，否则为进程控制
        value   信号量的初始值， 0 代表阻塞， 1 代表运行

int sem_wait(sem_t *sem);
    函数作用为检测指定信号量是否有资源可用，若无资源可用会阻塞等待，
    若有资源可用会自动的执行“ sem-1”的操作。
    所谓的“ sem-1”是与上述初始化函数中第三个参数值一致，成功执行会返回 0

int sem_post(sem_t *sem);
    会释放指定信号量的资源，执行“ sem+1”操作。


int sem_trywait(sem_t *sem);
    信号量申请(非阻塞方式)
    成功：返回 0

int sem_destory(sem_t *sem);
    信号量销毁
    成功：返回 0
*/

#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>

#define PTHREAD_CATCH(ret)       \
    if (ret != 0)                \
    {                            \
        perror("pthread_error"); \
        return -1;               \
    }

/*
引入：执行的顺序又不会受到控制

初始化一个信号量
    int sem_init(sem_t *sem,int pshared,unsigned int value);成功：返回 0
        sem     sem_t 类型指针
        pshared 传入 0 代表线程控制，否则为进程控制
        value   信号量的初始值， 0 代表阻塞， 1 代表运行

int sem_wait(sem_t *sem);
    函数作用为检测指定信号量是否有资源可用，若无资源可用会阻塞等待，
    若有资源可用会自动的执行“ sem-1”的操作。
    所谓的“ sem-1”是与上述初始化函数中第三个参数值一致，成功执行会返回 0

int sem_post(sem_t *sem);
    会释放指定信号量的资源，执行“ sem+1”操作。


int sem_trywait(sem_t *sem);
    信号量申请(非阻塞方式)
    成功：返回 0

int sem_destory(sem_t *sem);
    信号量销毁
    成功：返回 0
*/

sem_t sem1, sem2, sem3; // 申请的三个信号量变量

void *fun1(void *arg)
{
    sem_wait(&sem1); // 因sem1本身有资源，所以不被阻塞 获取后sem1-1 下次会会阻塞
    printf("%s:Pthread Come!\n", __FUNCTION__);
    sem_post(&sem2); // 使得sem2获取到资源
    pthread_exit(NULL);
}

void *fun2(void *arg)
{
    sem_wait(&sem2); // 因sem2在初始化时无资源会被阻塞，直至 fun1 释放
    printf("%s:Pthread Come!\n", __FUNCTION__);
    sem_post(&sem3); // 使得sem3获取到资源
    pthread_exit(NULL);
}

void *fun3(void *arg)
{
    sem_wait(&sem3); // 因sem3在初始化时无资源会被阻塞，，直至 fun2 释放
    printf("%s:Pthread Come!\n\n", __FUNCTION__);
    sem_post(&sem1); // 使得sem1获取到资源
    pthread_exit(NULL);
}

int main()
{
    int ret;
    pthread_t tid1, tid2, tid3;
    ret = sem_init(&sem1, 0, 1); // 初始化信号量1 并且赋予其资源
    PTHREAD_CATCH(ret);
    ret = sem_init(&sem2, 0, 0); // 初始化信号量2 让其阻塞
    PTHREAD_CATCH(ret);
    ret = sem_init(&sem3, 0, 0); // 初始化信号3 让其阻塞
    PTHREAD_CATCH(ret);

    int flag = 0;
    while (1)
    {

        ret = pthread_create(&tid1, NULL, fun1, NULL); // 创建线程1
        PTHREAD_CATCH(ret);
        ret = pthread_create(&tid2, NULL, fun2, NULL); // 创建线程2
        PTHREAD_CATCH(ret);
        ret = pthread_create(&tid3, NULL, fun3, NULL); // 创建线程3
        PTHREAD_CATCH(ret);
        if (flag >= 5)
            break;
        flag++;
    }
    /*回收线程资源*/
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);

    /*销毁信号量*/
    sem_destroy(&sem1);
    sem_destroy(&sem2);
    sem_destroy(&sem3);

    return 0;
}