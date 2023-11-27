#include <stdio.h>
#include <pthread.h>
// #include <stdint.h>
// #include <unistd.h>

#define PTHREAD_CATCH(ret)        \
    if (ret != 0)                 \
    {                             \
        perror("pthread_create"); \
        return -1;                \
    }


/**

互斥锁（`pthread_mutex_t`）和自旋锁（`pthread_spinlock_t`）是两种用于线程同步的机制，它们有一些关键的区别：

1. **等待方式**:
   - 互斥锁：当一个线程尝试获取一个被其他线程持有的互斥锁时，它会被阻塞，直到锁可用。这种方式称为阻塞式等待。
   - 自旋锁：线程在尝试获取锁时会不断地自旋，而不是被阻塞。自旋锁适用于短时间内可以获取锁的情况。

2. **性能**:
   - 互斥锁：适合长时间的锁占用和释放，因为它能够让等待的线程进入休眠状态，从而减少 CPU 的占用。但在高竞争情况下，线程的频繁切换可能会引起性能问题。
   - 自旋锁：适合短时间内的锁占用和释放，因为它减少了线程切换的开销。但在长时间等待的情况下，自旋锁会占用大量 CPU 时间。

3. **适用场景**:
   - 互斥锁：适用于锁占用和释放时间较长的情况，或者需要等待其他资源（如文件、网络连接等）的情况。
   - 自旋锁：适用于锁占用和释放时间非常短暂，或者在多处理器系统中，当线程被阻塞的时间非常短。

4. **线程睡眠**:
   - 互斥锁：当线程尝试获取锁时，如果锁不可用，线程会进入休眠状态，不会占用 CPU。
   - 自旋锁：当线程尝试获取锁时，如果锁不可用，线程会持续自旋，占用 CPU 时间。

总之，选择使用互斥锁还是自旋锁取决于您的应用程序的特性和性能需求。
互斥锁适用于大部分情况，因为它可以防止线程占用 CPU 时间，但在某些特定情况下，自旋锁可能更有效，因为它避免了线程切换的开销。


*/

static int commonSource = 0;
static pthread_spinlock_t lock;

void *funAdd(void *arg)
{
    pthread_spin_lock(&lock);
    int *value = (int *)arg;
    (*value)++;
    pthread_spin_unlock(&lock);
    printf("%s: tid = %lu, arguments = %d, arguments address = %p\n",
           __FUNCTION__, (unsigned long)pthread_self(), *value, arg);
}

void *funSub(void *arg)
{
    pthread_spin_lock(&lock);
    int *value = (int *)arg;
    (*value)--;
    pthread_spin_unlock(&lock);
    printf("%s: tid = %lu, arguments = %d, arguments address = %p\n",
           __FUNCTION__, (unsigned long)pthread_self(), *value, arg);
}

int main(void)
{
    pthread_t tid[200];
    int ret;

    // 初始化自旋锁，以确保多个线程可以共享
    pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);

    for (int i = 0; i < 100; i++)
    {
        ret = pthread_create(&tid[i], NULL, funAdd, (void *)&commonSource);
        PTHREAD_CATCH(ret);
    }

    for (int i = 100; i < 200; i++)
    {
        ret = pthread_create(&tid[i], NULL, funSub, (void *)&commonSource);
        PTHREAD_CATCH(ret);
    }

    // 等待所有线程完成
    for (int i = 0; i < 200; i++)
    {
        pthread_join(tid[i], NULL);
    }

    printf("%s: commonSource = %d, commonSource address = %p\n", __FUNCTION__, commonSource, &commonSource);

    // 销毁自旋锁
    pthread_spin_destroy(&lock);

    return 0;
}
