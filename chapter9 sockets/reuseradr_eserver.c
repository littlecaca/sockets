#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>

/**
 * Hello World Server!
 */



#define BUFSIZE 50
#define TRUE 1
#define FALSE 0

void error_handling(const char *message)
{
    fprintf(stderr, "%s:%s\n", message, strerror(errno));   
    exit(1);
}

int main(int argc, char const *argv[])
{
    int serv_sock;
    int clnt_sock;

    typedef struct sockaddr_in Addr;
    Addr serv_addr, clnt_addr;
    size_t str_len;
    int optlen;
    socklen_t clnt_addr_size;
    int option;

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
    
    
    /**
     * 将SO_REUSEADDR设置为1，可以使在time-wait状态下的端口被绑定给新的套接字
     */
    optlen = sizeof(option);
    option = TRUE;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void *)&option, optlen);


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

    clnt_addr_size = sizeof(clnt_addr);

    for (int i = 0; i < 5; ++i)
    {
        // accept()
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1)
            error_handling("accept() error");
        else
            printf("Connected client %d\n", i + 1);
        // echo()
        while ((str_len = read(clnt_sock, buf, BUFSIZE)))
        {
            if (str_len == -1)
                error_handling("read() error");
            write(clnt_sock, buf, str_len);
        }

        close(clnt_sock);
    }
    close(serv_sock);

    return 0;
}
