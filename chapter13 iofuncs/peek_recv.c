#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>

/**
 * 检查输入缓冲区的服务器端，实现只读取数据，不删除
 */
#define BUFSIZE 30

int serv_sock;
int recv_sock;

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char const *argv[])
{
    struct sockaddr_in recv_addr, clnt_addr;
    socklen_t adr_sz;
    int str_len, state;
    char buf[BUFSIZE];
    if (argc != 2)
    {
        printf("Usage : %s <port> \n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");
    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    recv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&recv_addr, sizeof(recv_addr)) == -1)
        error_handling("bind(does) error");
    if (listen(serv_sock, 15) == -1)
        error_handling("listen() error");

    // accept
    adr_sz = sizeof(clnt_addr);
    recv_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &adr_sz);

    while ((str_len = recv(recv_sock, buf, BUFSIZE - 1, MSG_PEEK | MSG_DONTWAIT)))
    {
        if (str_len > 0)
            break;
    }

    buf[str_len] = 0;
    printf("Buffering %d bytes: %s\n", str_len, buf);
    if ((str_len = recv(recv_sock, buf, BUFSIZE - 1, 0)))
    {
        buf[str_len] = 0;
        printf("Read again: %s\n", buf);
    }
    close(recv_sock);
    close(serv_sock);

    return 0;
}
