#include <unistd.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    int gval = 10, lval = 11;

    int pid;
    if (fork() == 0)
    {
        // 子进程
        lval += 10;
    }
    else
        lval -= 10;
    printf("gval: %d, lval: %d\n", gval, lval);

    return 0;
}
