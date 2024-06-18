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
        fputs("Input Expression (<cnt> <sign> <operands...>): ", stdout);
        
        fgets(buf, BUFSIZE - 1, stdin);
        
        if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
            break;
        if (write(sock, buf, strlen(buf)) == -1)
            error_handling("write() error");
        if ((str_len = read(sock, buf, BUFSIZE - 1)))
        {
            if (str_len == -1)
                error_handling("read() error");
            buf[str_len] = 0;
            printf("The answer is: %d\n", atoi(buf));
        }

    }
    close(sock);

    return 0;
}
