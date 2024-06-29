#include <stdio.h>
#include <string.h>

int main() {
    char str[] = "Hello, world! This is an example.";
    const char delim[] = " ,.!"; // 分隔符是空格、逗号、点和感叹号
    char *token;

    // 获取第一个令牌
    token = strtok(str, delim);

    // 继续获取其他令牌
    while (token != NULL) {
        printf("Token: %s\n", token);
        token = strtok(NULL, delim);
    }

    return 0;
}
