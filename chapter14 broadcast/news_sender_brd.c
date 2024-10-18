#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30

void errorHandling(const char *message);

int main(int argc, char const *argv[])
{
    int send_sock;
    struct sockaddr_in broad_adr;
    FILE *fp;
    char buf[BUF_SIZE];
    int bcast = 1;

    if (argc != 3)
    {
        printf("Usage : %s <BroadIP> <port> \n", argv[0]);
        exit(1);
    }
    send_sock = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&broad_adr, 0, sizeof(broad_adr));
    broad_adr.sin_family = AF_INET;
    broad_adr.sin_addr.s_addr = inet_addr(argv[1]);
    broad_adr.sin_port = htons(atoi(argv[2]));

    // (必须)设置套接字允许广播
    setsockopt(send_sock, SOL_SOCKET, SO_BROADCAST,
        (void *)&bcast, sizeof(bcast));

    if ((fp = fopen("news.txt", "r")) == NULL)
        errorHandling("fopen() error");

    while (!feof(fp))
    {
        fgets(buf, BUF_SIZE, fp);
        sendto(send_sock, buf, strlen(buf),
            0, (struct sockaddr *)&broad_adr, sizeof(broad_adr));
    }

    fclose(fp);
    close(send_sock);
    return 0;
}

void errorHandling(const char *message)
{
    perror(message);
    fputc('\n', stderr);
    exit(1);
}
