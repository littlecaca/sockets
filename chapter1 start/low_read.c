#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

void error_handling(const char *message)
{
    perror(message);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char const *argv[])
{
    int fd;
    char buf[30];
    
    
    fd = open("data.txt", O_RDONLY);

    if (fd == -1)
        error_handling("open() error");
    
    printf("file descriptor: %d\n", fd);

    if (read(fd, buf, sizeof(buf) - 1) == -1)
        error_handling("read() error");
    
    printf("The content in data.txt: %s", buf);
    
    close(fd);
    return 0;
}
