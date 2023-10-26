#include <stdio.h>
#include <pthread.h>

#define  PTHREAD_CATCH(ret) \
    if(ret != 0){   \
        perror("pthread_create");   \
        return -1;  \
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
    与 pthread_join 一致
*/
 void doTimer(int second)
{
    for (int i = 1; i <= second; i++)
    {
        sleep(1);
        printf("%s计时：%d\n",__FUNCTION__,i);
    }
}

void timer(int second)
{
    pthread_t tid;
    int ret = pthread_create(&tid,NULL,doTimer,second);
    PTHREAD_CATCH(ret);
}

void *fun1(void *arg)
{
    static int temp = 0; 
    // int temp = 0;// 不加 static ，temp存放在栈区，跟随函数一起被释放，加上static ，temp 在bbs区，不会被释放，可以访问
    temp = *(int *)arg;
    temp += 100;
    printf("%s:Addr = %p temp = %d\n", __FUNCTION__, &temp, temp);
    sleep(5);
    pthread_exit((void *)&temp); // 将变量 tmp 取地址转化为 void*类型传出
}




int main(void)
{
    
    pthread_t tid1;
    int a = 50;
    void *Tmp = NULL;
   
    timer(5);
    int ret = pthread_create(&tid1,NULL,fun1,(void *) &a);
    PTHREAD_CATCH(ret);
    

    pthread_join(tid1,&Tmp);// 阻塞式：线程资源回收完成才继续执行
    printf("%s:Addr = %p Val = %d\n",__FUNCTION__,Tmp,*(int *)Tmp);
    return 0;
}

/*
fun1:Addr = 0000000000407030 temp = 150
doTimer计时：1
doTimer计时：2
doTimer计时：3
doTimer计时：4
main:Addr = 0000000000407030 Val = 150
*/
