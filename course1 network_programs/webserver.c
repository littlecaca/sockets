#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "webserver.h"

int http_request(struct Conn *conn)
{
    set_event(conn->fd, EPOLLOUT, EPOLL_CTL_MOD);
    conn->status = 0;
    conn->wlength = 0;
    return 0;
}

int http_response(struct Conn *conn)
{
    const char *file = "index.html";
    int file_fd;
    if (conn->status == 0)
    {
        file_fd = open(file, O_RDONLY);
        if (file_fd == -1)
        {
            log_error("open() fails");
            return 1;
        }
        conn->file_fd = file_fd;
    }
    else
    {
        file_fd = conn->file_fd;
    }

    if (conn->status == 0)
    {
        struct stat filestat = {0};
        fstat(file_fd, &filestat);
        int sended = snprintf(conn->wbuffer, BUFFER_LENGTH,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Accept-Ranges: bytes\r\n"
            "Content-Length: %ld\r\n\r\n",
            filestat.st_size);
        conn->wlength = sended;
        conn->status = 1;
    }
    else if (conn->status == 1)
    {
        ssize_t recved = read(file_fd, conn->wbuffer, BUFFER_LENGTH);
        if (recved == 0)
        {
            close(file_fd);
            conn->status = 2;
        }
        if (recved < 0)
        {
            close(file_fd);
            log_error("read() fails");
            conn->status = 2;
        }
        conn->wlength = recved;
    }
    return 0;
}