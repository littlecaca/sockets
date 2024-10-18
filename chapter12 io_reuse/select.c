#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUFSIZE 30

int main(int argc, char const *argv[])
{
    fd_set reads, temps;
    int result, str_len;
    char buf[BUFSIZE];
    struct timeval timeout;

    FD_ZERO(&reads);
    FD_SET(0, &reads);

    // 调用select函数之后，timeout将被替换为超时前剩余时间，因此
    // 应该在循环体内设置timeout的值
    // timeout.tv_sec = 5;
    // timeout.tv_usec = 5000;

    while (1)
    {
        temps = reads;
        timeout.tv_sec = 5;
        result = select(1, &temps, 0, 0, &timeout);
        if (result == -1)
        {
            puts("select() error");
            break;
        }
        else if (result == 0)
            puts("time out");
        else
        {
            if (FD_ISSET(0, &temps))
            {
                str_len = read(0, buf, BUFSIZE - 1);
                buf[str_len] = 0;
                printf("message from console: %s", buf);
            }
        }
    }

    return 0;
}
