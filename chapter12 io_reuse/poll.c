#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/poll.h>

#define BUFSIZE 30

int main(int argc, char const *argv[])
{
    // 创建监听事件集合
    struct pollfd pfd;
    // 监听读事件
    pfd.fd = 0;
    pfd.events |= POLLIN;

    int result, str_len;
    char buf[BUFSIZE];

    while (1)
    {
        result = poll(&pfd, 1, 1000 * 60); // 60s
        if (result == -1)
        {
            puts("select() error");
            break;
        }
        else if (result == 0)
            puts("time out");
        else
        {
            if (pfd.revents & POLLIN)
            {
                str_len = read(pfd.fd, buf, sizeof buf - 1);
                buf[str_len] = 0;
                printf("message from console: %s", buf);
            }
        }
    }

    return 0;
}
