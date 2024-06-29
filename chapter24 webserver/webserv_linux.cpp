#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 1024
#define SMALL_BUF 100
 
enum http_wrong_code
{
    HTTP_WC_400,
    HTTP_WC_404,
};

void *request_handler(void *arg);
void send_data(FILE *fp, const char *ct, const char *file_name);
const char *content_type(const char *file);
void send_error(FILE *fp, http_wrong_code code);
void error_handling(const char *message);


static const char *wrong_message[] = 
{
    "400 Bad Request",
    "404 Not Found"
};

int main(int argc, char const *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;
    char buf[BUF_SIZE];
    pthread_t t_id;
    
    if (argc != 2)
    {
        printf("Usage : %s <port> \n", argv[0]);
        exit(-1);
    }

    if ((serv_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        error_handling("socket() error");
    int option = 1;

    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");
    
    if (listen(serv_sock, 15) == -1)
        error_handling("listen() error");

    while (1)
    {
        clnt_addr_size = sizeof(clnt_addr);
        if ((clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size)) == -1)
            error_handling("accept() error");
        printf("Connected client: %s:%d\n", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
        if (pthread_create(&t_id, NULL, &request_handler, (void *)&clnt_sock) != 0)
            error_handling("pthread_create() error");
        pthread_detach(t_id);
    }
    close(serv_sock);

    return 0;
}

void *request_handler(void *arg)
{
    int clnt_sock = *(int *)arg;
    char req_line[SMALL_BUF];
    FILE *clnt_read;
    FILE *clnt_write;

    char method[10];
    char ct[15];
    char file_name[30];

    clnt_read = fdopen(clnt_sock, "r");
    clnt_write = fdopen(dup(clnt_sock), "w");

    fgets(req_line, SMALL_BUF, clnt_read);
    if (strstr(req_line, "HTTP/") == NULL)
    {
        send_error(clnt_write, HTTP_WC_400);
        fclose(clnt_read);
        fclose(clnt_write);
        return NULL;
    }

    strcpy(method, strtok(req_line, " /"));
    strcpy(file_name, strtok(NULL, " /"));
    strcpy(ct, content_type(file_name));

    if (strcmp(method, "GET") != 0)
    {
        send_error(clnt_write, HTTP_WC_400);
        fclose(clnt_read);
        fclose(clnt_write);
        return NULL;
    }

    fclose(clnt_read);
    send_data(clnt_write, ct, file_name);
    return NULL;
}

void send_data(FILE *fp, const char *ct, const char *file_name)
{

    FILE *send_file;
    send_file = fopen(file_name, "r");
    struct stat st;
    if (send_file == NULL)
    {
        send_error(fp, HTTP_WC_404);
        return;
    }
    stat(file_name, &st);

    char protocol[] = "HTTP/1.0 200 OK\r\n";
    char server[] = "Server:Linux Web Server \r\n";
    char cnt_type[SMALL_BUF];
    char cnt_len[SMALL_BUF];
    char buf[BUF_SIZE];
    
    sprintf(cnt_len, "Content-length:%ld\r\n", st.st_size);
    sprintf(cnt_type, "Content-type:%s\r\n\r\n", ct);


    fputs(protocol, fp);
    fputs(server, fp);
    fputs(cnt_len, fp);
    fputs(cnt_type, fp);

    while (fgets(buf, BUF_SIZE, send_file) != NULL)
    {
        fputs(buf, fp);
        fflush(fp);
    }

    fflush(fp);
    fclose(fp);
}

const char *content_type(const char *file)
{
    char extension[SMALL_BUF];
    char file_name[SMALL_BUF];
    strcpy(file_name, file);
    strtok(file_name, ".");
    strcpy(extension, strtok(NULL, "."));

    if (!strcmp(extension, "html") || !strcmp(extension, "htm"))
        return "text/html";
    else
        return "text/plain";
}

void send_error(FILE *fp, http_wrong_code code)
{
    fprintf(fp, "HTTP/1.0 %s\r\n", wrong_message[code]);
    fputs("Server:Linux Web Server\r\n", fp);
    fputs("Content-length:2048\r\n", fp);
    fputs("Content-type:text/html\r\n\r\n", fp);
    
    char content[] = "<html><head><title>NETWORK</title></head>"
        "<body><font size=+5><br>发生错误！查看请求文件名和请求方式！"
        "</font></body></html>";
    fputs(content, fp);
    fflush(fp);
}

void error_handling(const char *message)
{
    perror(message);
    fputc('\n', stderr);
    exit(-1);
}
