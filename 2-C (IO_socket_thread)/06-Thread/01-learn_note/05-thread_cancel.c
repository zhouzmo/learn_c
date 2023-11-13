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

    result = pthread_create(&thread, NULL, thread_function, NULL);
    if (result != 0)
    {
        perror("pthread_create");
        return 1;
    }

    sleep(1); 
    pthread_cancel(thread);
     // thread_function:start...	tid=1	,arg =0
    // 无  sleep(1);  无输出


    return 0;
}
