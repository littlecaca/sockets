#include <stdio.h>
#include <string.h>


#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    struct sockaddr_in local_addr = {0}, peer_addr = {0};
    int sock_fd = socket(PF_INET, SOCK_STREAM, 0);

    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(2001);

    if (bind(sock_fd, (struct sockaddr *)&local_addr, sizeof local_addr) == -1)
    {
        perror("bind()");
        close(sock_fd);
        return 1;
    }

    peer_addr.sin_family = AF_INET;
    peer_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    peer_addr.sin_port = htons(2000);

    char buf[100];
    int received;

    while (1)
    {
        if (connect(sock_fd, (struct sockaddr *)&peer_addr, sizeof peer_addr) == -1)
        {
            usleep(50);
            continue;
        }


        if ((received = read(sock_fd, buf, sizeof buf - 1)) != 0)
        {
            if (received == -1)
            {
                perror("read()");
                close(sock_fd);
                return 1;
            }

            buf[received] = 0;
            
            printf("from peer:\n%s\n", buf);
        }

        close(sock_fd);
        break;
    }

    return 0;
}
