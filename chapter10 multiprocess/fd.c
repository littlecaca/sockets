#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>


int main(int argc, char const *argv[])
{
    pid_t pid;
    int sock;
    
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (fork() == 0)
        printf("child process fd: %d\n", sock);
    else
        printf("parent process fd: %d\n", sock);
    
    close(sock);
    return 0;
}
