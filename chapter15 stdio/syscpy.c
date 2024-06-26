#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

// 使用一个非常小的用户缓冲区
#define BUF_SIZE 3

/**
 * 利用系统函数复制文件
 */
int main(int argc, char const *argv[])
{
    int fd1, fd2;
    int len;
    char buf[BUF_SIZE];

    fd1 = open("news.txt", O_RDONLY);
    fd2 = open("cpy.txt", O_CREAT | O_TRUNC | O_WRONLY);

    while ((len = read(fd1, buf, sizeof(buf))) > 0)
        write(fd2, buf, len);

    close(fd1);
    close(fd2);

    return 0;
}
