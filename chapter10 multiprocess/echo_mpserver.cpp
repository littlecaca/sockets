#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <wait.h>

/**
 * Hello World Server!
 */

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void read_childproc(int sig)
{
    int status;
    pid_t id = waitpid(-1, &status, WNOHANG);
    if (WIFEXITED(status))
    {
        printf("Removed proc id: %d\n", id);
        printf("Child send %d\n", WEXITSTATUS(status));
    }
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
    pid_t pid;

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


    // set sigaction
    struct sigaction act;
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    sigaction(SIGCHLD, &act, 0);

    for (int i = 0; i < 50; ++i)
    {
        // accept()
        clnt_addr_size = sizeof(clnt_addr);
        // 这里由于子进程结束会唤醒挂起的主进程，而其本身没有收到连接请求，所以会失败
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
        
        if (clnt_sock == -1)
            continue;
        else
            printf("Connected client %d\n", i + 1);

        pid = fork();
        if (pid == -1)
        {
            close(clnt_sock);
            continue;
        }

        if (pid == 0)
        {
            // 只会减少文件计数，不会导致操作系统删除套接字
            close(serv_sock);
            // echo()
            while ((str_len = read(clnt_sock, buf, BUFSIZE)))
            {
                if (str_len == -1)
                    error_handling("read() error");
                write(clnt_sock, buf, str_len);
            }

            close(clnt_sock);
            return 0;
        }
        else
            close(clnt_sock);
    }
    close(serv_sock);

    return 0;
}
