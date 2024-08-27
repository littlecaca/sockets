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
    int sock;
    int snd_buf, rcv_buf;
    socklen_t optlen;
    int state;

    optlen = sizeof(snd_buf);
    // sock = socket(PF_INET, SOCK_STREAM, 0);
    sock = socket(PF_INET, SOCK_DGRAM, 0);

    state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void *)&snd_buf, &optlen);

    if (state == -1)
        errorHandling("getsockopt() error");
    printf("Output buffer size: %d\n", snd_buf);

    optlen = sizeof(rcv_buf);
    state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void *)&rcv_buf, &optlen);
    if (state == -1)
        errorHandling("getsockopt() error");

    printf("Input buffer size: %d\n", rcv_buf);
    close(sock);

    return 0;
}
