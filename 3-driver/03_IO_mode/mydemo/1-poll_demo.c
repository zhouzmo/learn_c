#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/poll.h>

int main() {

    /*
    struct pollfd {
    int fd;       // 要监测的文件描述符
    short events; // 要监测的事件类型
    short revents; // 实际发生的事件类型
    };
    */
    struct pollfd fds[2];
    int timeout_ms = 5000;  // 超时时间，以毫秒为单位

    // 监测标准输入（0号文件描述符）是否可以读取
    fds[0].fd = 0;  // 标准输入
    fds[0].events = POLLIN;  // 监测可读事件

    // 监测标准输出（1号文件描述符）是否可以写入
    fds[1].fd = 1;  // 标准输出
    fds[1].events = POLLOUT;  // 监测可写事件

    // 使用 poll 函数进行监测
    /*

    用法：`poll` 函数用于等待一组文件描述符上的事件。

    int poll(struct pollfd fds[], nfds_t nfds, int timeout);

    | 参数      | 描述                                                         |
    | --------- | ------------------------------------------------------------ |
    | `fds`     | 一个指向 `struct pollfd` 数组的指针，其中包含要监视的文件描述符和所需的事件信息。 |
    | `nfds`    | 要监视的文件描述符的数量。                                   |
    | `timeout` | 超时时间（以毫秒为单位）。如果设置为 `-1`，表示一直等待直到有事件发生。
                    如果设置为 `0`，表示立即返回而不等待。其他正整数值表示等待的毫秒数。 |
    | 返回值     | - 如果成功，返回已准备好的文件描述符数量。                     
                <br> - 如果超时，返回 `0`。                                       
                <br> - 如果出现错误，返回 `-1` 并设置 `errno` 来指示错误。 |

    函数说明：
    - `poll` 函数用于等待一组文件描述符上的事件。
    在 `fds` 数组中设置每个文件描述符的监视事件以及相应的返回事件，
    然后通过 `timeout` 参数指定等待的超时时间。
    - `nfds` 参数表示 `fds` 数组的大小，即要监视的文件描述符的数量。
    - `timeout` 参数控制等待的时间。如果将其设置为 `-1`，则表示一直等待，
    直到有事件发生。如果设置为 `0`，则表示立即返回，不等待。
    其他正整数值表示等待的毫秒数。

    - 函数返回时，会返回已准备好的文件描述符的数量（即有事件发生的文件描述符数），
    或者如果超时则返回 `0`，或者如果出现错误则返回 `-1` 并设置 `errno` 以指示错误原因。
    在你的代码片段中，`poll` 函数会等待 `fds` 数组
    中前两个文件描述符上的事件，并将结果存储在 `num_ready` 中，
    `timeout_ms` 参数表示等待的毫秒数。如果 `timeout_ms` 为负数，表示一直等待。
        
    */
    int num_ready = poll(fds, 2, timeout_ms);

    if (num_ready == -1) {
        perror("poll");
    } else if (num_ready == 0) {
        printf("No events occurred within the timeout period.\n");
    } else {
        for (int i = 0; i < 2; i++) {
            if (fds[i].revents & POLLIN) {
                printf("File descriptor %d is ready for reading.\n", fds[i].fd);
            }
            if (fds[i].revents & POLLOUT) {
                printf("File descriptor %d is ready for writing.\n", fds[i].fd);
            }
        }
    }

    return 0;
}
