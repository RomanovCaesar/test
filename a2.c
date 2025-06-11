#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return 1;
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    } else if (pid > 0) {
        // 父进程
        close(pipefd[0]); // 关闭读端

        // 写入字符串
        write(pipefd[1], argv[1], strlen(argv[1]) + 1);

        close(pipefd[1]); // 写完后关闭写端
    } else {
        // 子进程
        close(pipefd[1]); // 关闭写端

        char buffer[1024];
        ssize_t len = read(pipefd[0], buffer, sizeof(buffer));
        if (len > 0) {
            // 保证字符串以 null 结尾
            buffer[len] = '\0';
            printf("Received: %s\n", buffer);
        }

        close(pipefd[0]); // 读取完后关闭读端
    }

    return 0;
}
