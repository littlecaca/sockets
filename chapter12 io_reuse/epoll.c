#include <sys/epoll.h>
#include <stdio.h>
#include <unistd.h>


int main()
{
    int epfd;
    int result;
    int str_len;
    char buf[1024];

    // 创建epoll
    epfd = epoll_create(1);

    // 注册事件
    struct epoll_event event;
    event.data.fd = 0;
    event.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, 0, &event);
    
    // 接收就绪事件的集合
    struct epoll_event ready_events[1];

    while (1)
    {
        result = epoll_wait(epfd, ready_events, 1, 5000);
        if (result == -1)
        {
            puts("select() error");
            break;
        }
        else if (result == 0)
            puts("time out");
        else
        {
            if (ready_events[0].events & EPOLLIN)
            {
                str_len = read(ready_events[0].data.fd, buf, sizeof buf - 1);
                buf[str_len] = 0;
                printf("message from console: %s", buf);
            }
        }
    }

    return 0;
}