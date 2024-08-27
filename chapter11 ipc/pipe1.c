#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>


#define BUFSIZE 30

int main(int argc, char const *argv[])
{
    /**
     * 一个管道单向通信
     */
    int fds[2];

    char str[] = "Who are you?";
    char buf[BUFSIZE];
    pid_t pid;
    int strlen;

    pipe(fds);

    pid = fork();
    if (pid == 0)
    {
        write(fds[1], str, sizeof(str));
        close(fds[1]);
    }
    else
    {
        while ((strlen = read(fds[0], buf, BUFSIZE)))
        {
            if (strlen == -1)
            {
                printf("read() error");
                exit(1);
            }
            puts(buf);
        }
    }
    return 0;
}
