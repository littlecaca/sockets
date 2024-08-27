#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

void error_handling(const char *message)
{
    perror(message);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char const *argv[])
{
    int fd;
    char buf[] = "Let's go!\n";
    fd = open("data.txt", O_CREAT | O_TRUNC | O_WRONLY);
    if (fd == -1)
        error_handling("open() error");
    
    // 3
    printf("file descriptor: %d\n", fd);

    if (write(fd, buf, sizeof(buf) - 1) == -1)
        error_handling("write() error");
    close(fd);
    return 0;
}
