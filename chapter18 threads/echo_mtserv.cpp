#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

/**
 * Hello World Server!
 */

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void *handle_echo(void *arg);
void setnonblockingmode(int fd);

#define BUFSIZE 100

static char buf[BUFSIZE];
sem_t sem_capa;

int main(int argc, char const *argv[])
{
    int serv_sock;
    int clnt_sock;

    typedef struct sockaddr_in Addr;
    Addr serv_addr, clnt_addr;
    size_t str_len;
    socklen_t clnt_addr_size;
    pthread_t t_id;


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

    // initialze semaphore
    sem_init(&sem_capa, 0, 1);

    for (int i = 0; i < 500; ++i)
    {
        // accept()
        clnt_addr_size = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1)
            error_handling("accept() error");
        else
            printf("Connected client %d sock_fd %d\n", i + 1, clnt_sock);
        // echo()
        pthread_create(&t_id, NULL, &handle_echo, (void *)&clnt_sock);
        pthread_detach(t_id);
    }
    close(serv_sock);

    return 0;
}


void *handle_echo(void *arg)
{
    int sock = *(int *)arg;
    setnonblockingmode(sock);
    int str_len;
    while (1)
    {
        sem_wait(&sem_capa);
        str_len = read(sock, buf, BUFSIZE);
        if (str_len > 0)
            write(sock, buf, str_len);
        sem_post(&sem_capa);

        if (str_len < 0)
        {
            if (errno != EAGAIN)   
            {
                perror("read() error");
                break;
            }
        }
        else if (str_len == 0)
            break;
    }
    close(sock);
    return NULL;
}

void setnonblockingmode(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}
