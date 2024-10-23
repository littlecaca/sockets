#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Base64编码函数
char *base64_encode(const unsigned char *input, int length) {
    BIO *bmem, *b64;
    BUF_MEM *bptr;
    char *output;

    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);

    BIO_write(b64, input, length);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);

    output = (char *)malloc(bptr->length + 1);
    memcpy(output, bptr->data, bptr->length);
    output[bptr->length] = 0;

    BIO_free_all(b64);

    return output;
}

int main() {
    // 原始字符串
    const char *str = "WQ6cxlerA04+eJkdHviGQ==258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    unsigned char hash[SHA_DIGEST_LENGTH];  // SHA-1输出20字节的哈希值

    // 计算SHA-1哈希
    SHA1((unsigned char*)str, strlen(str), hash);

    // 对哈希值进行Base64编码
    char *encoded = base64_encode(hash, SHA_DIGEST_LENGTH);

    // 输出Base64编码结果
    printf("Sec-WebSocket-Accept: %s\n", encoded);

    free(encoded);  // 释放编码后的内存
    return 0;
}
