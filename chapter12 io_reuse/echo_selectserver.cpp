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

/**
 * 使用多进程工作的能够保存回声的回声服务器端
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
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    struct timeval timeout;
    size_t fd_max, str_len, fd_num, i;
    socklen_t clnt_addr_size;
    fd_set reads, cpy_reads;

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

    FD_ZERO(&reads);
    FD_SET(serv_sock, &reads);
    fd_max = serv_sock;

    while(1)
    {
        cpy_reads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        if ((fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout)) == -1)
            break;
        if (fd_num == 0)
            continue;

        for (i = 0; i < fd_max + 1; ++i)
        {
            if (FD_ISSET(i, &cpy_reads))
            {
                if (i == serv_sock)
                {
                    clnt_addr_size = sizeof(clnt_addr);
                    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
                    FD_SET(clnt_sock, &reads);
                    if (fd_max < clnt_sock)
                        fd_max = clnt_sock;
                    printf("Connected client: %d\n", clnt_sock);
                }
                else
                {
                    str_len = read(i, buf, BUFSIZE);
                    if (str_len == 0)
                    {
                        FD_CLR(i, &reads);
                        close(i);
                        printf("Closed client: %zu\n", i);
                    }
                    else
                        write(i, buf, str_len);
                }
            }
        }
    }
    close(serv_sock);
    return 0;
}
