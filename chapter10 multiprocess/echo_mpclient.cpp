#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <wait.h>
#include <signal.h>

/**
 * Hello World Server!
 */

#define BUFSIZE 50

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void read_childproc(int sig)
{
    int status;
    pid_t id = waitpid(-1, &status, WNOHANG);
    if (WIFEXITED(status))
    {
        printf("Removed proc id: %d\n", id);
        printf("Child send %d\n", WEXITSTATUS(status));
    }
}

void read_routine(int sock, char *buf)
{
    int str_len;
    while ((str_len = read(sock, buf, BUFSIZE - 1)))
    {
        if (str_len == -1)
            error_handling("read() error");
        printf("Message from the server: \n");
        buf[str_len] = 0;
        printf("%s", buf);
    }
    close(sock);
}

void write_routine(int sock, char *buf)
{
    while (1)
    {
        // fputs("Input Message (Q to quit): ", stdout);
    
        fgets(buf, BUFSIZE, stdin);
    
        if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
        {
            shutdown(sock, SHUT_WR);
            break;
        }
        if (write(sock, buf, strlen(buf)) == -1)
            error_handling("write() error");
    }
    close(sock);
}

int main(int argc, char const *argv[])
{
    int sock;
    int str_len;
    typedef struct sockaddr_in Addr;
    Addr serv_addr;
    pid_t pid;
    

    char buf[BUFSIZE];

    if (argc != 3)
    {
        printf("Usage : %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    // sigaction
    struct sigaction act;
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, 0);

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
    
    if (fork() == 0)
        write_routine(sock, buf);   // write
    else
        read_routine(sock, buf);

    return 0;
}
