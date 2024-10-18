#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

void errorHandling(const char *message)
{
    perror(message);
    fputc('\n', stderr);
    exit(1);
}

#define BUFSIZE 1000

int main(int argc, char const *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t addr_size;
    char buf[BUFSIZE];

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        errorHandling("socket() error");
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
    
    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        errorHandling("bind() error");
    
    if (listen(serv_sock, 15) == -1)
        errorHandling("listen() error");
    

    for (int i = 0; i < 100; ++i)
    {
        clnt_sock =  accept(serv_sock, (struct sockaddr *)&clnt_addr, &addr_size);
        if (clnt_sock == -1)
            errorHandling("accept() error");

        printf("Connection %d\n", i + 1);
    
        /**
         * 处理请求
         */ 
        if (read(clnt_sock, buf, 4) == -1)
            errorHandling("read() error");
        int len = *((int *)buf);
        for (int recv_len = 0; recv_len < len;)
        {
            int recv_cnt = read(clnt_sock, &buf[recv_len], len - recv_len);
            if (recv_cnt == -1)
                errorHandling("read() error");
            recv_len += recv_cnt;
        }
        buf[len] = 0;
        
        FILE *fp = fopen(buf, "r");
        if (fp)
        {
            while (fgets(buf, BUFSIZE, fp))
            {
                uint32_t line_len = strlen(buf);
                write(clnt_sock, (char *)&line_len, 4);
                write(clnt_sock, buf, line_len);
            }
            fclose(fp);
        }
        else
        {
            len = -1;
            write(clnt_sock, (char *)&len, 4);
        }
        close(clnt_sock);
    }
    close(serv_sock);
    return 0;
}
