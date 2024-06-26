#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

/**
 * Hello World Server!
 */

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

#define BUFSIZE 50

int main(int argc, char const *argv[])
{
    int serv_sock;
    int clnt_sock;

    typedef struct sockaddr_in Addr;
    Addr serv_addr, clnt_addr;
    size_t str_len;
    socklen_t clnt_addr_size;
    FILE *readfp, *writefp;

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

    clnt_addr_size = sizeof(clnt_addr);

    for (int i = 0; i < 5; ++i)
    {
        // accept()
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1)
            error_handling("accept() error");
        else
            printf("Connected client %d\n", i + 1);
        writefp = fdopen(clnt_sock, "w");

        readfp = fdopen(dup(clnt_sock), "r");
        // echo()

        fputs("Hi client? \n", writefp);
        fputs("It's a nice day? right?\n", writefp);
        fputs("Come with me!? ha?\n", writefp);

        fflush(writefp);
        // 调用shutdown函数时，无论复制出多少文件描述符都进入半关闭状态，同时传递EOF
        shutdown(fileno(writefp), SHUT_WR);
        fclose(writefp);

        fgets(buf, BUFSIZE, readfp);
        fputs(buf, stdout);

        fclose(readfp);
    }
    close(serv_sock);

    return 0;
}
