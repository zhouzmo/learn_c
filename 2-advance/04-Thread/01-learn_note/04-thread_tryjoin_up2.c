#include <stdio.h>
#include <pthread.h>

void *thread_function(void *arg)
{
    // 线程的工作内容
    // 线程的工作内容
    printf("%s:start...\ttid=%lu\t,arg =%d\n",
           __FUNCTION__, (unsigned long)pthread_self(), (int)(long)arg);
}

int main()
{
    pthread_t thread;
    int result;

    // 创建一个分离状态的线程
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    result = pthread_create(&thread, &attr, thread_function, NULL);
    if (result != 0)
    {
        perror("pthread_create");
        return 1;
    }

    sleep(1); 
    // thread_function:start...	tid=1	,arg =0
    // 无  sleep(1);  无输出
    pthread_attr_destroy(&attr);

    // 主线程不需要显式等待子线程退出

    return 0;
}
