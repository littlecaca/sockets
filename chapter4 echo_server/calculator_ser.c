#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

/**
 * Hello World Server!
 */

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

#define BUFSIZE 50

int cal(const int *operands, int cnt, char sign)
{
    int ans = operands[0];
    if (sign == '-')
        for (int i = 1; i < cnt; ++i)
            ans -= operands[i];
    else if (sign == '+')
        for (int i = 1; i < cnt; ++i)
            ans += operands[i];
    else if (sign == '*')
        for (int i = 1; i < cnt; ++i)
            ans *= operands[i];
    else
        error_handling("cal() error");
    return ans;
}


int main(int argc, char const *argv[])
{
    int serv_sock;
    int clnt_sock;

    typedef struct sockaddr_in Addr;
    Addr serv_addr, clnt_addr;
    size_t str_len;
    socklen_t clnt_addr_size;

    char buf[BUFSIZE];

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    // socket()
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");
    
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    /**
     * INADDR_ANY表示可自动获取运行服务器端的计算机ip地址
     */
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    // bind()
    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    // listen()
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    clnt_addr_size = sizeof(clnt_addr);

    for (int i = 0; i < 5; ++i)
    {
        // accept()
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1)
            error_handling("accept() error");
        else
            printf("Connected client %d\n", i + 1);
        // cal
        char sign = 0;
        int operands[30];
        int cur_num = 0;
        int operand_num = -1;
        int temp_num = 0;
        int flag = 1;

        while (str_len = read(clnt_sock, buf, BUFSIZE))
        {
            if (str_len == -1)
                error_handling("read() error");
            int cur = 0;
            while (cur < str_len)
            {
                // 读取一个完整的数字
                if (buf[cur] == ' ' || buf[cur] == '\n')
                {
                    temp_num *= flag;
                    if (operand_num == -1)
                        operand_num = temp_num;
                    else if (sign == 0)
                        sign = temp_num;
                    else
                    {
                        operands[cur_num++] = temp_num;
                        if (cur_num == operand_num)
                        {
                            char ans[32];
                            sprintf(ans, "%d", cal(operands, operand_num, sign));
                            write(clnt_sock, ans, strlen(ans));
                            sign = 0;
                            operand_num = -1;
                            cur_num = 0;
                        }
                    }
                    temp_num = 0;
                    flag = 1;
                }
                else
                {
                    if (buf[cur] >= '0' && buf[cur] <= '9')
                        temp_num = temp_num * 10 + buf[cur] - '0';
                    else if (buf[cur] == '-' && sign != 0)
                        flag = -flag;
                    else
                        temp_num = buf[cur];
                }
                ++cur;
            }
        }

        close(clnt_sock);
    }
    close(serv_sock);

    return 0;
}
