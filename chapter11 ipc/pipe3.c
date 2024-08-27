#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>


#define BUFSIZE 30

int main(int argc, char const *argv[])
{
    /**
     * 两个管道双向通信
     */
    int fds1[2], fds2[2];


    char str1[] = "Who are you?";
    char str2[] = "Thank you for your message";
    char buf[BUFSIZE];
    pid_t pid;

    pipe(fds1);
    pipe(fds2);

    pid = fork();
    if (pid == 0)
    {
        write(fds1[1], str1, sizeof(str1));
        read(fds2[0], buf, BUFSIZE);
        printf("Child output: %s\n", buf);
    }
    else
    {
        read(fds1[0], buf, BUFSIZE);
        printf("Parent output: %s\n", buf);
        write(fds2[1], str2, sizeof(str2));
    }
    return 0;
}
