#include "public.h"

int sockfd;

//发送线程的任务函数
void *pthread_send(void *arg) {
    char buf[DATA_SIZE];
    while(1) {
        fgets(buf, sizeof(buf), stdin);
        buf[strcspn(buf, "\n")] = '\0';
        if(strcmp(buf, "exit") == 0) {
            exit(0);
        }
        send(sockfd, buf, sizeof(buf), 0);        
    }
}

//接收线程的任务函数
void *pthread_recv(void *arg) {
    char buf[DATA_SIZE];
    while(1) {
        ssize_t size = recv(sockfd, buf,sizeof(buf), 0);
        if(size > 0) {
            system("date '+%Y-%m-%d %H:%M:%S'");
            if(strncmp(buf, "客户端", strlen("客户端")) == 0) {
                printf("%s\n", buf);
            }
            else {
                printf("服务器:%s\n", buf);
            }
            memset(buf, 0, sizeof(buf));         
        }
        else if(size == 0) {
            printf("服务器已关闭\n");
            exit(0);
        }
  
    }
}

int main() {
    //创建一个 IPv4 的 TCP 套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1) {
        perror("socket failed");
        return -1;
    }

    SA_IN seraddr;
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(PORT_NUM);
    seraddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    int ret = connect(sockfd, (SA *)&seraddr, sizeof(seraddr));
    if(ret == -1) {
        perror("connect failed");
        return -1;
    }
    printf("成功连接服务器,等待返回客户端编号\n");

     //创建线程
    pthread_t tid_send, tid_recv;
    pthread_create(&tid_send, NULL, pthread_send, NULL);
    pthread_create(&tid_recv, NULL, pthread_recv, NULL);

    pthread_join(tid_send, NULL);
    pthread_join(tid_recv, NULL);

}