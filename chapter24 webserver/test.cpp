#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

int main() {

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    int sock2 = dup(sock);
    printf("%d, %d\n", sock, sock2);

    return 0;
}
