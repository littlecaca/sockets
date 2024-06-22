#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
    /**
     * 销毁僵尸进程
     * 使用wait函数时，如果没有已终止的子进程，那么程序将阻塞直到有子进程终止。
     */
    int pid = fork();
    int status;
    if (pid == 0)
        printf("start Child process...\n");
    else
    {
        printf("child process id: %d\n", pid);
        printf("start Parent process...\n");
        wait(&status);
        if (WIFEXITED(status))
            printf("Child send one: %d\n", WEXITSTATUS(status));
        sleep(30);
    }

    if (pid == 0)
        printf("end child process\n");
    else 
        printf("end parent process\n");

    return 0;
}
