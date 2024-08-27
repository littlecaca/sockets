#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>


int main(int argc, char const *argv[])
{
    uint16_t i = htons(atoi("9190"));
    printf("%d\n", i);
    return 0;
}
