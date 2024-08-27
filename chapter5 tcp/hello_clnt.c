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
    int sock;
    struct sockaddr_in serv_addr;
    char message[] = "Hello! Cool! Server!";
    socklen_t addr_size;
    char buf[BUFSIZE];

    if (argc != 3)
    {
        printf("Usage : %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));



    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        errorHandling("connect() error");
    
    printf("Message from server: \n");
    if (read(sock, buf, 4) == -1)
        errorHandling("read() error");
    int len = *((int *)buf);
    for (int recv_len = 0; recv_len < len;)
    {
        int recv_cnt = read(sock, buf, BUFSIZE - 1);
        if (recv_cnt == -1)
            errorHandling("read() error");
        buf[recv_cnt] = 0;
        printf("%s", buf);
        recv_len += recv_cnt;
    }
    putchar('\n');

    len = strlen(message);
    *((uint32_t *)buf) = len;
    strcpy(buf + 4, message);
    write(sock, buf, len + 4);

    close(sock);
    
    return 0;
}
