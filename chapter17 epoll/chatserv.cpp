#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <wait.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include <signal.h>

/**
 * 采用条件触发的epoll的聊天室服务端
 */

#define BUFSIZE 50
#define EPOLL_SIZE 50
#define CLIENT_SIZE 50

static struct epoll_event *ep;

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void clear(int sig)
{
    free(ep);
    fputs("clear\n", stdout);
    exit(1);
}

bool exist(char *clients, int i)
{
    return clients[i / 8] & (1 << (i % 8));
}

void use(char *clients, int i)
{
    clients[i / 8] |= (1 << (i % 8));
}

void clear(char *clients, int i)
{
    clients[i / 8] &= !(1 << (i % 8));
}

int main(int argc, char const *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    struct timeval timeout;
    ssize_t fd_max = 0, str_len, fd_num, i;
    socklen_t clnt_addr_size;
    struct epoll_event event;
    int epfd;
    char clients[CLIENT_SIZE / 8 + 1];
    
    char buf[BUFSIZE];

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    // socket()
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");
    

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    /**
     * INADDR_ANY表示可自动获取运行服务器端的计算机ip地址
     */
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    // bind()
    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    // listen()
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    // sigaction to free the dynamic memory
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = clear;
    sigaction(SIGINT, &act, 0);

    // epoll
    epfd = epoll_create(EPOLL_SIZE);
    if (epfd == -1)
        error_handling("epoll_create() error");
    event.events = EPOLLIN;
    event.data.fd = serv_sock;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event) == -1)
        error_handling("epoll_ctl() error");

    ep = (struct epoll_event *)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);
    
    while(1)
    {
        if ((fd_num = epoll_wait(epfd, ep, EPOLL_SIZE, 500)) == -1)
            error_handling("epoll_wait() error");

        for (i = 0; i < fd_num; ++i)
        {
            struct epoll_event *cur = ep + i;

            if (cur->data.fd == serv_sock)
            {
                clnt_addr_size = sizeof(clnt_addr);
                clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
                if (clnt_sock < CLIENT_SIZE)
                {
                    if (clnt_sock > fd_max) fd_max = clnt_sock;
                    use(clients, clnt_sock);
                    // 注册套接字
                    event.events = EPOLLIN;
                    event.data.fd = clnt_sock;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
                    printf("Connected client: %s\n", inet_ntoa(clnt_addr.sin_addr));
                }
                else
                {
                    write(clnt_sock, "[Server] The room is full!\n", strlen("[Server] The room is full!\n"));
                    close(clnt_sock);
                }
            }
            else
            {
                clnt_sock = cur->data.fd;
                str_len = read(clnt_sock, buf, BUFSIZE);
                if (str_len == -1)
                    error_handling("read() error");
                if (str_len == 0)
                {
                    // read the EOF
                    epoll_ctl(epfd, EPOLL_CTL_DEL, clnt_sock, NULL);
                    clear(clients, clnt_sock);
                    close(clnt_sock);
                    printf("Closed client: %d\n", clnt_sock);
                }
                else
                {
                    for (int k = 0; k <= fd_max; ++k)
                    {
                        if (exist(clients, k))
                            write(k, buf, str_len);
                    }
                }
            }
        }
    }
    close(serv_sock);
    close(epfd);
    free(ep);
    return 0;
}
