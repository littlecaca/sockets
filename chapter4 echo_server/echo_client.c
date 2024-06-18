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

#define BUFSIZE 50

int main(int argc, char const *argv[])
{
    int sock;
    int str_len;
    typedef struct sockaddr_in Addr;
    Addr serv_addr;
    

    char buf[BUFSIZE];

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

    serv_addr.sin_port = htons(atoi(argv[2]));

    // connect
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");
    else
        printf("connecting........\n");
    
    // echo
    while (1)
    {
        fputs("Input Message (Q to quit): ", stdout);
        
        fgets(buf, BUFSIZE, stdin);
        
        if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
            break;
        if ((str_len = write(sock, buf, strlen(buf))) == -1)
            error_handling("write() error");
        
        size_t read_len = 0;
        size_t recv_cnt;

        printf("Message from the server: \n");
        while (read_len < str_len)
        {
            recv_cnt = read(sock, buf, str_len - read_len);
            if (recv_cnt == -1)
                error_handling("read() error");
            read_len += recv_cnt;
            buf[recv_cnt] = 0;
            printf("%s", buf);
        }
    }
    close(sock);

    return 0;
}
