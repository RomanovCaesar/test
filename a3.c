#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>

#define MSGSIZE 128
#define TO_CHILD 1
#define TO_PARENT 2

// 定义消息结构体
struct msgbuf {
    long mtype;       // 消息类型
    int msg_id;       // 消息编号
    int number;       // 传输的数据
};

int main() {
    int msqid;
    pid_t pid;

    // 创建消息队列
    if ((msqid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT)) == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    // 创建子进程
    pid = fork();
    if (pid < 0) {
        perror("fork");
        msgctl(msqid, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // 子进程
        struct msgbuf msg_recv, msg_send;
        while (1) {
            // 接收来自父进程的消息
            if (msgrcv(msqid, &msg_recv, sizeof(msg_recv) - sizeof(long), TO_CHILD, 0) == -1) {
                perror("msgrcv child");
                exit(EXIT_FAILURE);
            }

            // 接收到的数据
            int num = msg_recv.number;
            int id = msg_recv.msg_id;

            // 准备响应消息
            msg_send.mtype = TO_PARENT;
            msg_send.msg_id = id;
            msg_send.number = num * num;

            // 发回父进程
            if (msgsnd(msqid, &msg_send, sizeof(msg_send) - sizeof(long), 0) == -1) {
                perror("msgsnd child");
                exit(EXIT_FAILURE);
            }
        }
    } else {
        // 父进程
        struct msgbuf msg_send, msg_recv;
        int num, id = 0;

        // 读取输入直到 EOF
        while (scanf("%d", &num) != EOF) {
            msg_send.mtype = TO_CHILD;
            msg_send.msg_id = id;
            msg_send.number = num;

            // 发送给子进程
            if (msgsnd(msqid, &msg_send, sizeof(msg_send) - sizeof(long), 0) == -1) {
                perror("msgsnd parent");
                break;
            }

            id++;
        }

        // 接收所有结果
        for (int i = 0; i < id; i++) {
            if (msgrcv(msqid, &msg_recv, sizeof(msg_recv) - sizeof(long), TO_PARENT, 0) == -1) {
                perror("msgrcv parent");
                break;
            }

            printf("msg_id=%d, result=%d\n", msg_recv.msg_id, msg_recv.number);
        }

        // 删除消息队列
        msgctl(msqid, IPC_RMID, NULL);

        // 杀死子进程
        kill(pid, SIGKILL);
        wait(NULL);
    }

    return 0;
}
