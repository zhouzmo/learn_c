#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

#define MAX_EVENTS 5

int main(int argc, char **argv) {
   
    int fd, epoll_fd, event_count;
    struct epoll_event event, events[MAX_EVENTS];

    if (argc != 2) {
        printf("Usage: %s <dev>\n", argv[0]);
        return -1;
    }

    fd = open(argv[1], O_RDWR | O_NONBLOCK);
    if (fd < 0) {
        printf("Failed to open device %s\n", argv[1]);
        return -1;
    }

    // 创建 epoll 实例
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        return -1;
    }

    // 将设备文件描述符添加到 epoll 实例中
    event.events = EPOLLIN;
    event.data.fd = fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1) {
        perror("epoll_ctl");
        return -1;
    }

    while (1) {
        event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, 3000); // 阻塞等待 3s
        if (event_count > 0) {
            printf("Success epoll_wait() events = %d\n", event_count);
            for (int i = 0; i < event_count; ++i) {
                if (events[i].events & EPOLLIN) {
                    // 如果有可读事件发生
                    struct input_event ev;
                    ssize_t read_ret;
                    while ((read_ret = read(fd, &ev, sizeof(ev))) == sizeof(ev)) {
                        printf("Read event: type = 0x%x, code = 0x%x, value = 0x%x\n", ev.type, ev.code, ev.value);
                    }
                }
            }
        } else if (event_count == -1) {
            perror("epoll_wait");
            return -1;
        } else {
            printf("Timeout epoll_wait()\n");
        }
    }

    close(fd);
    return 0;
}
