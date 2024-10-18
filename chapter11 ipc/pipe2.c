#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>


#define BUFSIZE 30

int main(int argc, char const *argv[])
{
    /**
     * 一个管道双向通信
     */
    int fds[2];

    char str1[] = "Who are you?";
    char str2[] = "Thank you for your message";
    char buf[BUFSIZE];
    pid_t pid;

    pipe(fds);

    pid = fork();
    if (pid == 0)
    {
        write(fds[1], str1, sizeof(str1));
        sleep(2);
        read(fds[0], buf, BUFSIZE);
        printf("Child output: %s\n", buf);
    }
    else
    {
        read(fds[0], buf, BUFSIZE);
        printf("Parent output: %s\n", buf);
        write(fds[1], str2, sizeof(str2));
        sleep(3);

    }
    return 0;
}
