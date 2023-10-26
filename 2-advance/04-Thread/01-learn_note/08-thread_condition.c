#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>

#define PTHREAD_CATCH(ret)       \
    if (ret != 0)                \
    {                            \
        perror("pthread_error"); \
        return -1;               \
    }

/*
引入：执行的顺序又不会受到控制

// 初始化条件变量
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *cond_attr);//cond_at
tr 通常为 NULL
// 销毁条件变量
int pthread_cond_destroy(pthread_cond_t *cond);
这些函数成功时都返回 0
*/

static char g_buf[1000];
static pthread_mutex_t g_tMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_tConVar = PTHREAD_COND_INITIALIZER;

static void *my_thread_func(void *data)
{
    while (1)
    {
        // sleep(1);
        /* 等待通知 */
        // while (g_hasData == 0);
        pthread_mutex_lock(&g_tMutex); // 获取锁
        pthread_cond_wait(&g_tConVar, &g_tMutex); // 等待通知数据写入完成
        /* 打印 */
        printf("recv: %s\n", g_buf);
        printf("please input:");
        pthread_mutex_unlock(&g_tMutex);
    }

    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t tid;
    int ret;
    char buf[1000];

    /* 1. 创建"接收线程" */
    ret = pthread_create(&tid, NULL, my_thread_func, NULL);
    if (ret)
    {
        printf("pthread_create err!\n");
        return -1;
    }

    /* 2. 主线程读取标准输入, 发给"接收线程" */
    printf("please input:");
    while (1)
    {

        fgets(buf, 1000, stdin);// 读入数据到 buf
        pthread_mutex_lock(&g_tMutex);
        memcpy(g_buf, buf, 1000); 
        pthread_cond_signal(&g_tConVar); /* 通知接收线程,g_buf 数据已经读入 */
        pthread_mutex_unlock(&g_tMutex);
    }
    return 0;
}
/*
please input:1
recv: 1

please input:2
recv: 2

please input:3
recv: 3

please input:eqweqweqwe
recv: eqweqweqwe

please input:



*/