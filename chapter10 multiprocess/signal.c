#include <signal.h>
#include <unistd.h>
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
    /**
     * 信号于signal函数 
     * 
     * void (*signal(int signo, void (&func)(int)))(int);
     * 
     * SIGALRM : 已到通过调用alarm函数注册的时间
     * SIGINT : 输入CTRL+C
     * SIGCHLD : 子进程终止
     */
    int i;
    signal(SIGALRM, timeout);
    signal(SIGINT, keycontrol);
    alarm(2);

    for (i = 0; i < 30; ++i)
    {
        puts("wait...");
        sleep(100);
    }
    
    return 0;
}
