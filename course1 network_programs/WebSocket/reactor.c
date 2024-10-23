#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/signal.h>

#include "webserver.h"

void error_handling(const char *message)
{
    fprintf(stderr, "%s:%s\n", message, strerror(errno));   
    exit(1);
}

void log_error(const char *message)
{
    fprintf(stderr, "%s:%s\n", message, strerror(errno)); 
}


int init_listen(unsigned short port)
{
    int server_fd = socket(PF_INET, SOCK_STREAM, 0);
    
    if (server_fd == -1) error_handling("socket() fails");
    
    struct sockaddr_in serv_addr = {0};
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   	serv_addr.sin_port = htons(port);
    
    if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof serv_addr) == -1)
    {
        error_handling("bind() fails");
    }
    
    if (listen(server_fd, 10) == -1)
    {
        error_handling("listen() fails");
    }
    return server_fd;
}

static struct Conn conn_list[CONNECTION_LENGTH];
static int epfd;
static int client_cnt = 0;
static struct timeval last_time = {};

int set_event(int fd, int event, int flag)
{
    struct epoll_event et;
    et.events = event;
    et.data.fd = fd;
    while (1)
    {
	    int res = epoll_ctl(epfd, flag, fd, &et);
    	if (res == -1)
    	{
	        if (errno == EINTR || errno == EAGAIN)
                continue;
            error_handling("epoll_ctl() fails");
            return -1;
	    }
	    else break;
    }
    return 0;
}

void closefd(int fd)
{
    // close
    set_event(fd, 0, EPOLL_CTL_DEL);
    if (close(fd) == -1) error_handling("close() fails");
    printf("close client %d\n", fd);
}

int accept_cb(int fd);
int send_cb(int fd);

int recv_cb(int fd)
{
    struct Conn *pconn = conn_list + fd;
    int clnt_fd = fd;

    int read_len = recv(clnt_fd, pconn->rbuffer, BUFFER_LENGTH - 1, 0);

    if (read_len == 0)
    {
        closefd(clnt_fd);
        return 0;
    }

    if (read_len == -1) 
    {
        set_event(clnt_fd, 0, EPOLL_CTL_DEL);
        if (close(clnt_fd) == -1) error_handling("close() fails");
        log_error("recv fails()");
        return 0;
    }

    pconn->rlength = read_len;
    pconn->rbuffer[read_len] = '\0';

#ifdef WEBSOCKET
        ws_request(pconn);
#else
        http_request(pconn);
#endif
    set_event(fd, EPOLLOUT, EPOLL_CTL_MOD);
    return 0;
}

int registerCallback(int fd, int flag)
{
    memset(conn_list + fd, 0, sizeof (struct Conn));
    conn_list[fd].fd = fd;
    if (flag)
        conn_list[fd].recv_callback = accept_cb;
    else
        conn_list[fd].recv_callback = recv_cb;

    conn_list[fd].send_callback = send_cb;

    set_event(fd, EPOLLIN, EPOLL_CTL_ADD);
    return 0;
}

int send_cb(int fd)
{
    struct Conn *pconn = conn_list + fd;
#ifdef WEBSOCKET
        ws_response(pconn);
#else
        http_response(pconn);
#endif

    int clnt_fd = pconn->fd;

    if (pconn->wlength > 0)
    {
        send(clnt_fd, pconn->wbuffer, pconn->wlength, 0);
    }
    set_event(clnt_fd, EPOLLIN, EPOLL_CTL_MOD);
    
    return 0;
}

int accept_cb(int fd)
{
    struct sockaddr_in clnt_addr = {0};
    socklen_t clnt_addr_size = sizeof (struct sockaddr_in);
    int clnt_fd = accept(fd, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
    if (clnt_fd == -1)
    {
        error_handling("accept() fails");
    }
    registerCallback(clnt_fd, 0);

    if (++client_cnt % 1000 == 0)
    {
        struct timeval now;
        gettimeofday(&now, NULL);

        int time_used = (now.tv_sec - last_time.tv_sec) * 1000 +  (now.tv_usec - last_time.tv_usec) / 1000;
        last_time = now;
        printf("totoal client count: %d, time used: %d\n", client_cnt, time_used);
    }
    return 0;
}

// server
int main()
{
    // ingore signal SIGPIPE
    signal(SIGPIPE, SIG_IGN);

    epfd = epoll_create1(0);

    int server_fd;
    unsigned short port = 2000;
    for (int i = 0; i < PORT_NUM; ++i)
    {
        server_fd = init_listen(port + i);
        registerCallback(server_fd, 1);
    }

    gettimeofday(&last_time, NULL);
    
    struct epoll_event events[READY_LENFTH] = {0};
    int nready;
    
    while (1)
    {
        nready = epoll_wait(epfd, events, READY_LENFTH, 5);
        if (nready == -1)
        {
			if (errno == EINTR || errno == EAGAIN) continue;
            error_handling("epoll_wait() fails");
        }

        for (int i = 0; i < nready; ++i)
        {
            int clnt_fd = events[i].data.fd;
            if (events[i].events & EPOLLIN)
            {
                conn_list[clnt_fd].recv_callback(clnt_fd);
            }
            if (events[i].events & EPOLLOUT)
            {
                conn_list[clnt_fd].send_callback(clnt_fd);
            }
        }
    }
    
    return 0;
}
 
