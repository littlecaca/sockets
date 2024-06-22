#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

void errorHandling(const char *message)
{
    perror(message);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char const *argv[])
{
    /**
     * 套接字类型只能在创建时决定，以后不能更改
     */
    int tcp_sock, udp_sock;
    int sock_type;
    socklen_t optlen;
    int state;

    optlen = sizeof(sock_type);
    tcp_sock = socket(PF_INET, SOCK_STREAM, 0);
    udp_sock = socket(PF_INET, SOCK_DGRAM, 0);
    printf("SOCK_STREAM: %d\n", tcp_sock);
    printf("SOCK_DGRAM: %d\n", udp_sock);

    state = getsockopt(tcp_sock, SOL_SOCKET, SO_TYPE, &sock_type, &optlen);
    if (state == -1)
        errorHandling("getsockopt() error");
    printf("Socket type one: %d\n", sock_type);

    state = getsockopt(udp_sock, SOL_SOCKET, SO_TYPE, &sock_type, &optlen);
    if (state == -1)
        errorHandling("getsockopt() error");
    printf("Socket type one: %d\n", sock_type);

    close(tcp_sock);
    close(udp_sock);
    return 0;
}
