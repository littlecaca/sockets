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
    uint32_t str_len;

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
    
    printf("Input the file name: \n");
    scanf("%s", buf);
    uint32_t len = strlen(buf);
    write(sock, (char *)&len, 4);
    write(sock, buf, len);

    printf("<<%s>>: \n", buf);


    while ((str_len = read(sock, buf, 4)))
    {
        if (str_len == -1)
            errorHandling("read() error");
        str_len = *((uint32_t *)buf);
        if (str_len == -1)
        {
            printf("The file cannot be found!\n");
            exit(1);
        }
        for (int recv_len = 0; recv_len < str_len;)
        {
            int recv_cnt = read(sock, &buf[recv_len], str_len - recv_len);
            if (recv_cnt == -1)
                errorHandling("read() error");
            recv_len += recv_cnt;
        }
        buf[str_len] = 0;
        printf("%s", buf);
    }   

    close(sock);
    
    return 0;
}
