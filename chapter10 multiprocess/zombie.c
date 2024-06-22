#include <unistd.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    /**
     * 制造僵尸进程
     */
    int pid = fork();
    if (pid == 0)
        printf("start Child process...\n");
    else
    {
        printf("child process id: %d\n", pid);
        printf("start Parent process...\n");
        sleep(30);
    }

    if (pid == 0)
        printf("end child process\n");
    else 
        printf("end parent process\n");

    return 0;
}
