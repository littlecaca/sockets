#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/**
 * 使用MSG_OOB可选项发送紧急消息
 */
#define BUFSIZE 50

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char const *argv[])
{
    int sock;
    struct sockaddr_in recv_addr;
    if (argc != 3)
    {
        printf("Usage : %s <ip> <port> \n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");
    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    recv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr *)&recv_addr, sizeof(recv_addr)) == -1)
        error_handling("connect() error");


    write(sock, "123", strlen("123"));

    close(sock);
    return 0;
}
