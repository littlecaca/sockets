#include <unistd.h>
#include <signal.h>
#include <stdio.h>

// signal handler
void timeout(int sig)
{
    if (sig == SIGALRM)
        puts("Time out!");
    alarm(2);
}

void keycontrol(int sig)
{
    if (sig == SIGINT)
        puts("CTRL + C Pressed");
}

int main(int argc, char const *argv[])
{
    int i;
    struct sigaction act;
    act.sa_handler = timeout;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    sigaction(SIGALRM, &act, 0);
    act.sa_handler = keycontrol;
    sigaction(SIGINT, &act, 0);

    
    alarm(2);

    for (i = 0; i < 3; ++i)
    {
        puts("wait...");
        sleep(30);
    }
    return 0;
}
