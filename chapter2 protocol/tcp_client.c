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
    perror(message);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char const *argv[])
{
    int sock;
    int str_len;
    typedef struct sockaddr_in Addr;
    Addr serv_addr;

    char message[30];

    if (argc != 3)
    {
        printf("Usage : %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    // socket()
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");
    
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    /**
     * 这里不能使用htonl，因为在32位及以上的机器上，会转换为32位整数
     * 而端口号都是16位
     */


    serv_addr.sin_port = htons(atoi(argv[2]));

    // connect
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");

    // read
    int idx = 0;
    while (str_len = read(sock, &message[idx++], 1))
    {
        if (str_len == -1)
            error_handling("read() error");
    }


    printf("Message from Server:\n%s\n", message);
    close(sock);

    return 0;
}
