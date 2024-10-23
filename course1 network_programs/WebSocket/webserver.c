#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#include "webserver.h"
#include "websocket.h"

#define DEBUG

#define WEBSOCKET_KEY_LENGTH 256

// 负责按照rfc6455的规定输出Sec-WebSocket-Accept的值
static int encode_key(unsigned char *key, size_t n, unsigned char *output)
{
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(key, n, hash);


    BIO *bmem, *b64;
    BUF_MEM *bptr;

    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);

    BIO_write(b64, hash, SHA_DIGEST_LENGTH);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);
    memcpy(output, bptr->data, bptr->length);
    // 这里切记是bptr->length字符数组的长度
    output[bptr->length - 1] = 0;

    BIO_free_all(b64);

    return 0;
}

int handshake(struct Conn *conn)
{
    // handshake
    unsigned char output[WEBSOCKET_KEY_LENGTH] = {0};
    unsigned char input[WEBSOCKET_KEY_LENGTH] = {0};
    char *key = strstr(conn->rbuffer, "Sec-WebSocket-Key:");
    if (!key)
    {
        conn->wlength = 0;
        return 1;
    }
    key += 19;
    int i = 0;
    while (*key != 0 && *key != ' ' && *key != '\r')
    {
        input[i++] = *key++;
    }
    strcpy((char *)&input[i], "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
    encode_key(input, strlen((char *)input), output);
    
    struct stat filestat = {0};
    int sended = snprintf(conn->wbuffer, BUFFER_LENGTH,
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: %s\r\n\r\n", (char *)output);

    printf("%s|||\n", output);

    conn->wlength = sended;
    return 0;
}

int ws_request(struct Conn *conn)
{
    printf("<<<<<input<<<<<\n %s\n", conn->rbuffer);
    if (conn->status == 0)
    {
        handshake(conn);
        conn->status = 1;
    }
    else if (conn->status == 1)
    {
        int ret = 0;
        conn->payload = decode_packet((unsigned char *)conn->rbuffer, conn->mask, conn->rlength, &ret);
        printf("data: %s, length: %d\n", conn->payload, ret);
        conn->wlength = ret;
        conn->status = 2;
    }
    return 0;
}

int ws_response(struct Conn *conn)
{
    if (conn->status == 2)
    {
        conn->wlength = encode_packet(conn->wbuffer, conn->mask, conn->payload, conn->wlength);
        conn->status = 1;
    }

    conn->wbuffer[conn->wlength] = 0;
    printf(">>>>output>>>>\n%s\n", conn->wbuffer);
    return 0;
}