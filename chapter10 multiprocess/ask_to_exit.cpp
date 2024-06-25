#include <string.h>
#include <wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 50

static int status = 1;

void ask_to_exit(int sig)
{
    status = 0;
    printf("Are you sure to exit? (y or n) :");
    char buf[BUFSIZE];
    fgets(buf, BUFSIZE, stdin);
    if (!strcmp(buf, "y\n") || !strcmp(buf, "Y\n"))
        exit(1);
    status = 1;
}

void act_for_alarm(int sig)
{
    if (status)
        printf("running...\n");
    alarm(1);
}

int main(int argc, char const *argv[])
{
    struct sigaction act;
    act.sa_handler = ask_to_exit;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, 0);
    
    act.sa_handler = act_for_alarm;
    sigaction(SIGALRM, &act, 0);

    while(1)
    {
        printf("running...\n");
        sleep(1);
    }
    return 0;
}
