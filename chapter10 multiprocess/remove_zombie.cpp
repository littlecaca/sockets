#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

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

int main(int argc, char const *argv[])
{
    pid_t pid;
    struct sigaction act;
    
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, 0);

    pid = fork();
    if (pid == 0)
    {
        puts("child process start...");
        puts("child process remove...");
    }
    else
    {
        puts("parent process start...");
        printf("child process: %d\n", pid);
        sleep(50);
        puts("parent process end");
    }
    return 0;
}
