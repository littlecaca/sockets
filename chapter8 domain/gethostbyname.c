#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>

void errorHandling(const char *message)
{
    perror(message);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char const *argv[])
{
    int i;
    struct hostent *host;
    if (argc != 2)
    {
        printf("Usage : %s <addr>\n", argv[0]);
        exit(1);
    }

    host = gethostbyname(argv[1]);
    if (!host)
        errorHandling("gethostbyname() error");
    
    printf("Official name: %s\n", host->h_name);
    for (i = 0; host->h_aliases[i]; ++i)
        printf("Alias %d: %s\n", i + 1, host->h_aliases[i]);
    printf("Address type: %s\n", 
        (host->h_addrtype == AF_INET) ? "AF_INET" : "AF_INET6");
    for(i = 0; host->h_addr_list[i]; ++i)
        printf("IP addr %d: %s\n", i + 1,
            inet_ntoa(*(struct in_addr *)host->h_addr_list[i]));
    return 0;
}
