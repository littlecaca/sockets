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
#include <signal.h>
#include <pthread.h>

/**
 * 使用多进程工作的能够保存回声的回声服务器端
 */

#define BUFSIZE 100
#define MAX_CLNT 256

void error_handling(const char *message);
void *handle_clnt(void *arg);
void send_msg(char *msg, int len);

static char clients[MAX_CLNT / 8 + 1];
static int fd_max = 0;
pthread_mutex_t mutex;

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

void clear(int sig)
{
    pthread_mutex_destroy(&mutex);
    exit(0);
}

int main(int argc, char const *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    struct timeval timeout;
    socklen_t clnt_addr_size;
    pthread_t t_id;
    const char *full_msg = "The links is full\n";

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
    
    while(1)
    {
        clnt_addr_size = sizeof(clnt_addr);
        if ((clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size)) == -1)
            error_handling("accept() error");

        pthread_mutex_lock(&mutex);
        if (clnt_sock > MAX_CLNT)
        {
            write(clnt_sock, full_msg, strlen(full_msg));
            close(clnt_sock);
            sleep(1);
            continue;
        }
        else
        {
            use(clients, clnt_sock);
            if (clnt_sock > fd_max) fd_max = clnt_sock;
        }
        pthread_mutex_unlock(&mutex);
        
        printf("Connected client: %d\n", clnt_sock);
        pthread_create(&t_id, NULL, &handle_clnt, &clnt_sock);
        // 从内存中完全销毁已终止的线程
        pthread_detach(t_id);
    }
    close(serv_sock);
    return 0;
}

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void *handle_clnt(void *arg)
{
    int sock = *(int *)arg;
    int str_len;
    char buf[BUFSIZE];
    while ((str_len = read(sock, (void *)buf, BUFSIZE)))
        send_msg(buf, str_len);
    close(sock);
    pthread_mutex_lock(&mutex);
    clear(clients, sock);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void send_msg(char *msg, int len)
{
    pthread_mutex_lock(&mutex);
    for (int i = 0; i <= fd_max; ++i)
    {
        if (exist(clients, i))
            write(i, msg, len);
    }
    pthread_mutex_unlock(&mutex);
}
