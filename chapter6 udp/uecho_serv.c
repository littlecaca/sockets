#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <string.h>


void errorHandling(const char *message)
{
    perror(message);
    fputc('\n', stderr);
    exit(1);
}

#define BUFSIZE 300
int main(int argc, char const *argv[])
{
    int serv_sock;
    char buf[BUFSIZE];

    int str_len;
    socklen_t clnt_addr_sz;

    struct sockaddr_in serv_addr, clnt_addr;

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (serv_sock == -1)
        errorHandling("socket() error");
    
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        errorHandling("bind() error");
    
    while (1)
    {
        clnt_addr_sz = sizeof(clnt_addr);
        str_len = recvfrom(serv_sock, buf, BUFSIZE, 0, (struct sockaddr *)&clnt_addr, &clnt_addr_sz);
        if (str_len == -1)
            errorHandling("recvfrom() error");
        if (sendto(serv_sock, buf, str_len, 0, (struct sockaddr *)&clnt_addr, clnt_addr_sz) == -1)
            errorHandling("sendto() error");
    }
    close(serv_sock);
    return 0;
}
