#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();  // 创建子进程

    if (pid < 0) {
        // fork 失败
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        // 子进程
        printf("[Child] My PID: %d, My PPID: %d\n", getpid(), getppid());
    } else {
        // 父进程
        printf("[Parent] My PID: %d, My child's PID: %d\n", getpid(), pid);
        int status;
        waitpid(pid, &status, 0);  // 等待子进程结束
        printf("[Parent] Child %d exited.\n", pid);
    }

    return 0;
}
