#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

// 使用一个非常小的用户缓冲区
#define BUF_SIZE 3

/**
 * 利用标准库函数复制文件
 */
int main(int argc, char const *argv[])
{
    FILE *fd1, *fd2;
    int len;
    char buf[BUF_SIZE];

    fd1 = fopen("news.txt", "r");
    fd2 = fopen("cpy.txt", "w");

    while (fgets(buf, BUF_SIZE, fd1))
        fputs(buf, fd2);

    fclose(fd1);
    fclose(fd2);

    return 0;
}
