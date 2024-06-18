#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

void errorHandling(const char *message)
{
    perror(message);
    fputc('\n', stderr);
    exit(1);
}

#define BUFSIZE 1000

int main(int argc, char const *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    char message[] = "Hello! Client!";
    socklen_t addr_size;
    char buf[BUFSIZE];

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        errorHandling("socket() error");
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
    
    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        errorHandling("bind() error");
    
    if (listen(serv_sock, 15) == -1)
        errorHandling("listen() error");
    

    for (int i = 0; i < 100; ++i)
    {
        clnt_sock =  accept(serv_sock, (struct sockaddr *)&clnt_addr, &addr_size);
        if (clnt_sock == -1)
            errorHandling("accept() error");

        printf("Connection %d\n", i + 1);
    
        int len = strlen(message);
        *((uint32_t *)buf) = len;
        strcpy(buf + 4, message);
        write(clnt_sock, buf, len + 4);

        if (read(clnt_sock, buf, 4) == -1)
            errorHandling("read() error");
        
        len = *((int *)buf);
        printf("Message from client: \n");
        for (int recv_len = 0; recv_len < len;)
        {
            int recv_cnt = read(clnt_sock, buf, BUFSIZE - 1);
            if (recv_cnt == -1)
                errorHandling("read() error");
            buf[recv_cnt] = 0;
            printf("%s", buf);
            recv_len += recv_cnt;
        }
        putchar('\n');

        close(clnt_sock);
    }
    close(serv_sock);
    return 0;
}
