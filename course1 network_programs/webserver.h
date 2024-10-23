#pragma once

#include <stdio.h>

#define BUFFER_LENGTH 819200
#define CONNECTION_LENGTH 256
#define READY_LENFTH 1024
#define PORT_NUM 2

typedef int (*RCallBack)(int fd);

struct Conn
{
    int fd;
    char rbuffer[BUFFER_LENGTH];
    char wbuffer[BUFFER_LENGTH];
    int rlength;
    int wlength;
    RCallBack send_callback;
    RCallBack recv_callback;
    int status;
    int file_fd;
};

int http_request(struct Conn *);
int http_response(struct Conn *);

int set_event(int fd, int event, int flag);

void error_handling(const char *message);

void log_error(const char *message);