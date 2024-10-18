#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

void errorHandling(const char *message);

#define BUF_SIZE 30

int main(int argc, char const *argv[])
{
    int fd;
    FILE *fp;
    fp = fopen("data.txt", "r");
    if (!fp)
        errorHandling("fopen() error");
    fd = fileno(fp);

    char buf[BUF_SIZE];
    int str_len;

    while ((str_len = read(fd, buf, BUF_SIZE)))
    {
        if (str_len == -1)
            errorHandling("read() error");
        write(1, buf, str_len);
    }
    fclose(fp);
    return 0;
}

void errorHandling(const char *message)
{
    perror(message);
    fputc('\n', stderr);
    exit(1);
}
