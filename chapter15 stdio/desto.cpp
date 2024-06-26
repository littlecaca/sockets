#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

void errorHandling(const char *message);

int main(int argc, char const *argv[])
{
    FILE *fp;
    int fd = open("data.txt", O_WRONLY | O_CREAT | O_TRUNC);
    if (fd == -1)
        errorHandling("open() error");
    
    fp = fdopen(fd, "w");
    fputs("Network C programming\n", fp);
    fclose(fp);
    return 0;
}

void errorHandling(const char *message)
{
    perror(message);
    fputc('\n', stderr);
    exit(1);
}
