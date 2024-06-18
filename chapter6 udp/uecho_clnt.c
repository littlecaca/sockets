#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <string.h>


void errorHandling(const char *message)
{
    perror(message);
    fputc('\n', stderr);
    exit(1);
}

#define BUFSIZE 300
int main(int argc, char const *argv[])
{
    int clnt_sock;
    char buf[BUFSIZE];

    int str_len;
    socklen_t from_addr_sz;

    struct sockaddr_in serv_addr, from_addr;

    if (argc != 3)
    {
        printf("Usage : %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    clnt_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (clnt_sock == -1)
        errorHandling("socket() error");
    
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    /**
     * 手动注册udp套接字的目标ip和端口
     * 这样就可以用write/read替代sendto/recvfrom
    */
    if (connect(clnt_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        errorHandling("connect() error");
    
    
    
    while (printf("> (q to quit):"), fgets(buf, BUFSIZE, stdin))
    {
        if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
            break;
        str_len = strlen(buf);
        // if (sendto(clnt_sock, buf, str_len, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        //     errorHandling("sendto() error");
        if (write(clnt_sock, buf, str_len) == -1)
            errorHandling("write() error");
    
        // str_len = recvfrom(clnt_sock, buf, BUFSIZE - 1, 0, (struct sockaddr *)&from_addr, &from_addr_sz);
        str_len = read(clnt_sock, buf, BUFSIZE - 1);
        if (str_len == -1)
            errorHandling("recvfrom() error");
        printf("Message from the server: \n");
        buf[str_len] = 0;
        printf("%s", buf);
    }

    close(clnt_sock);
    return 0;
}
