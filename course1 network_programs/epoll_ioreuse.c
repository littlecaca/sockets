#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

void error_handling(const char *message)
{
    fprintf(stderr, "%s:%s\n", message, strerror(errno));   
    exit(1);
}

// server
int main()
{
	int server_fd = socket(PF_INET, SOCK_STREAM, 0);
    
    if (server_fd == -1) error_handling("socket() fails");
    
    struct sockaddr_in serv_addr = { 0 }, clnt_addr = { 0 };
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   	serv_addr.sin_port = htons(2000);
    
    if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof serv_addr) == -1)
    {
        error_handling("bind() fails");
    }
    
    if (listen(server_fd, 10) == -1)
    {
        error_handling("listen() fails");
	}
    
    socklen_t clnt_addr_size = sizeof clnt_addr;

    char buf[1000];
    size_t read_len;
    
    struct epoll_event events[1024] = {0};
    int epfd = epoll_create1(0);
    struct epoll_event cur_event = {0};
    cur_event.events |= EPOLLIN;
    cur_event.data.fd = server_fd;

    epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &cur_event);
    int nready;
    
    while (1)
    {
        nready = epoll_wait(epfd, events, sizeof events, 5);
        if (nready == -1)
        {
            error_handling("select() fails");
        }

        for (int i = 0; i < nready; ++i)
        {
            if (events[i].data.fd == server_fd)
            {
                // accept
                int clnt_fd = accept(server_fd, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
                if (clnt_fd == -1)
                {
                    error_handling("accept() fails");
                }
                cur_event.events = EPOLLIN;
                cur_event.data.fd = clnt_fd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_fd, &cur_event);

                printf("accept new client %d\n", clnt_fd);
            }
            else
            {
                int clnt_fd = events[i].data.fd;
                read_len = recv(clnt_fd, buf, sizeof buf - 1, 0);
                if (read_len == 0)
                {
                    // close
                    if (close(i) == -1) error_handling("close() fails");
                    epoll_ctl(epfd, EPOLL_CTL_DEL, clnt_fd, &cur_event);
                    printf("close client %d\n", i);
                    continue;
                }

                if (read_len == -1) error_handling("recv() fails");

                buf[read_len] = '\0';
                printf("from %d: %s\n", clnt_fd, buf);
                // send
                send(i, buf, read_len, 0);
            }
        }
    }
    
    return 0;
}
 