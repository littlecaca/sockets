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
 * 能够接收out-of-band消息的服务端
 * 
 * 收到MSG_OOB紧急消息时，操作系统将产生SIGURG信号，并调用注册的信号处理函数 
 * 处理SIGURG信号必须指定处理信号的进程
 * 
 * TCP的紧急模式(Urgent mode)
 * 
 * MSG_OOB的真正意义在于督促数据接收对象尽快处理数据，TCP的”保持传输顺序“特性依然成立
 * 换言之，紧急消息的意义在于督促消息处理。
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

void usg_handler(int signo)
{
    int str_len;
    char buf[BUFSIZE];
    str_len = recv(recv_sock, buf, BUFSIZE - 1, MSG_OOB);
    buf[str_len] = 0;
    printf("Urgent message: %s\n", buf);
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

    // set sigaction
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = usg_handler;

    // accept
    adr_sz = sizeof(clnt_addr);
    recv_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &adr_sz);

    // bind signal to handler
    // 将套接字的拥有者改为getpid()返回值所指向的进程，也就是当前进程
    fcntl(recv_sock, F_SETOWN, getpid());
    state = sigaction(SIGURG, &act, 0);

    while ((str_len = recv(recv_sock, buf, BUFSIZE - 1, 0)))
    {
        if (str_len == -1)
            continue;
        buf[str_len] = 0;
        puts(buf);
    }
    close(recv_sock);
    close(serv_sock);

    return 0;
}
