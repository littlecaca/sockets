#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

void error_handling(const char *message)
{
    fprintf(stderr, "%s:%s\n", message, strerror(errno));   
    exit(1);
}

void *deal(void *args)
{
    int clnt_fd = *(int *)args;

    char buf[1000];

    int read_len;
    while ((read_len = recv(clnt_fd, buf, sizeof buf - 1, 0)) != 0)
    {
        if (read_len == -1)
        {
            error_handling("recv fails");
        }

        buf[read_len] = '\0';

        printf("from %d: %s\n", clnt_fd, buf);
            // send
        send(clnt_fd, buf, read_len, 0);
    }

        // close
    if (close(clnt_fd) == -1)
    {
        error_handling("close() fails");
    }

    printf("close client %d\n", clnt_fd);

    return NULL;
}

// server
int main()
{
	int server_fd = socket(PF_INET, SOCK_STREAM, 0);
    
    if (server_fd == -1) error_handling("socket() fails");
    
    struct sockaddr_in serv_addr = { 0 }, clnt_addr = { 0 };
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   	serv_addr.sin_port = htons(2000);
    
    if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof serv_addr) == -1)
    {
        error_handling("bind() fails");
    }
    
    if (listen(server_fd, 10) == -1)
    {
        error_handling("listen() fails");
	}
    
    
    socklen_t clnt_addr_size = sizeof clnt_addr;

    char buf[1000];
    size_t read_len;
    pthread_t tid;

    while (1)
    {
        // accept
        int clnt_fd = accept(server_fd, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
        if (clnt_fd == -1)
        {
            error_handling("accept() fails");
        }

        printf("accept new client %d\n", clnt_fd);

        int *fd = malloc(sizeof(int));
        *fd = clnt_fd;
        pthread_create(&tid, NULL, deal, fd);
        pthread_detach(tid);
    }
    
    return 0;
}