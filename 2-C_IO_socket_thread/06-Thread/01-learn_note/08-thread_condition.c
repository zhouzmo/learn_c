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
int pthread_cond_init(pthread_cond_t *cond, pthread_cond attr_t *cond_attr);
//cond_attr 通常为 NULL
// 销毁条件变量
int pthread_cond_destroy(pthread_cond_t *cond);
这些函数成功时都返回 0
*/

/*
当涉及到线程间的条件等待时，`pthread_cond_t` 结构和相关函数用于实现线程间的同步。下面是与 `pthread_cond_t` 相关的主要函数文档：

### 初始化条件变量

```c
int pthread_cond_init(pthread_cond_t *restrict cond, const pthread_condattr_t *restrict attr);
```

- `cond`：指向待初始化的条件变量的指针。
- `attr`：条件变量的属性。通常设置为 `NULL`。

返回值：成功初始化返回 0，失败返回错误码。

---

### 销毁条件变量

```c
int pthread_cond_destroy(pthread_cond_t *cond);
```

- `cond`：指向待销毁的条件变量的指针。

返回值：成功销毁返回 0，失败返回错误码。

---

### 等待条件变量满足

```c
int pthread_cond_wait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex);
```

- `cond`：指向条件变量的指针。
- `mutex`：指向已锁定的互斥锁的指针。

返回值：返回时不可预测。在返回之前，函数会原子地释放 `mutex`，并使调用线程阻塞，直到另一个线程调用 `pthread_cond_signal` 或 `pthread_cond_broadcast` 并且调用成功为止。

---

### 唤醒等待条件变量的线程

```c
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
```

- `cond`：指向条件变量的指针。

`pthread_cond_signal`：唤醒等待在 `cond` 上的一个线程。

`pthread_cond_broadcast`：唤醒等待在 `cond` 上的所有线程。

返回值：成功返回 0，失败返回错误码。

这些函数通常与互斥锁配合使用，允许线程在某个条件满足之前等待，然后在条件满足时被唤醒继续执行。

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