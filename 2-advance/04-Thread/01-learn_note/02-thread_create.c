#include <stdio.h>
#include <pthread.h>
#define  PTHREAD_CATCH(ret) \
    if(ret != 0){   \
        perror("pthread_create");   \
        return -1;  \
    }

    
void *fun1(void *arg)
{
    printf("%s: tid = %lu,arguments = %d,arguments address = %p\n",
           __FUNCTION__, (unsigned long)pthread_self(), *(int *)arg, arg);
}

void *fun2(void *arg)
{
    printf("%s: tid = %lu,arguments = %d,arguments address = %p\n",
           __FUNCTION__, (unsigned long)pthread_self(), (int)(long)arg, arg);
}

int main(void)
{

    pthread_t tid1, tid2;
    int a = 50;
    int ret = pthread_create(&tid1, NULL, fun1, (void *)&a);
     PTHREAD_CATCH(ret);
    ret = pthread_create(&tid2, NULL, fun2, (long)a);
     PTHREAD_CATCH(ret);

    sleep(1);
    printf("%s:a = %d Add = %p \n", __FUNCTION__, a, &a);

    return 0;
}
/*
fun1: tid = 1,arguments = 50,arguments address = 000000000061FE04        
fun2: tid = 2,arguments = 50,arguments address = 0000000000000032        
main:a = 50 Add = 000000000061FE04 

*/