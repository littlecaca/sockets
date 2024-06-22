#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    /**
     * 制造僵尸进程
     * waitpid函数不会造成父进程阻塞
     */
    int status;
    int pid = fork();
    if (pid == 0)
    {
        printf("start Child process...\n");
        sleep(15);
    }
    else
    {
        printf("child process id: %d\n", pid);
        printf("start Parent process...\n");
        // 即使没有终止的子进程也不会进入阻塞状态，而是返回0并退出函数
        while (!waitpid(-1, &status, WNOHANG))
        {
            sleep(3);
            puts("sleep 3 sec.");
        }

        if (WIFEXITED(status))
            printf("Child send %d \n", WEXITSTATUS(status));
    }

    if (pid == 0)
        printf("end child process\n");
    else 
        printf("end parent process\n");

    return 0;
}
