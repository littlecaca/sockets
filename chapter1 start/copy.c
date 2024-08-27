#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFSIZE 100

void errorHandling(const char *message)
{
    perror(message);
    fputc('\n', stderr);
    exit(1);
}

void low_copy(const char *fromFile, const char *toFile)
{
    int fromFd = open(fromFile, O_RDONLY);
    if (fromFd == -1)
        errorHandling("fromFd open() erorr");
    int toFd = open(toFile, O_CREAT | O_TRUNC | O_WRONLY);
    if (toFd == -1)
        errorHandling("toFd oepn() error");

    char buf[BUFSIZE];
    size_t strlen;
    while ((strlen = read(fromFd, buf, sizeof(buf))))
    {
        if (strlen == -1)
            errorHandling("read() error");
        if (write(toFd, buf, strlen) == -1)
            errorHandling("write() error");
    }

    close(fromFd);
    close(toFd);
}

void ansi_copy(const char *fromFile, const char *toFile)
{
    FILE *from = fopen(fromFile, "r");
    FILE *to = fopen(toFile, "w");
    if (!from || !to)
        errorHandling("fopen() error");

    char buf[BUFSIZE];

    while (fgets(buf, sizeof(buf), from))
    {
        if (fputs(buf, to) == EOF)
            errorHandling("fputs() erorr");
    }
    fclose(from);
    fclose(to);
}

int main(int argc, char const *argv[])
{
    const char *fromFIle, *toFile;
    char temp[30];

    if (argc == 2)
    {
        fromFIle = argv[1];
        strncpy(temp, argv[1], 25);
        strcat(temp, ".back");
        toFile = temp;
    }
    else if (argc == 3)
    {
        fromFIle = argv[1];
        toFile = argv[2];
    }
    else
    {
        printf("Usage : %s <fromFile> <toFile>\n", argv[0]);
        exit(1);
    }

    ansi_copy(fromFIle, toFile);

    
    return 0;
}
