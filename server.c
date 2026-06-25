#include "public.h"

int clifd[CLIENT_MAX_NUM] = {0};    //存储客户的套接字，如果为 0 则表示未连接
int cli_num = 0;                    //客户端在线数量

//发送线程的任务函数
void *pthread_send(void *arg) {
    char buf[DATA_SIZE];        //屏幕获取的数据
    char send_buf[DATA_SIZE];  //要发送的数据
    while(1) {
        if(cli_num > 0) {
            fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\n")] = '\0';
            if(strcmp(buf, "exit") == 0) {
                Server_Send(0, buf, clifd);
                exit(0);
            }
            
            //分离索引和数据
            int index = Sep_index_data(buf, send_buf);  
            if(index == 0) {
                Server_Send(index, send_buf, clifd);
            }
            else if(index > 0 && index <= CLIENT_MAX_NUM && clifd[index-1] != 0) {
                Server_Send(index, send_buf, clifd); 
            }
            else {
                printf("输入格式错误\n");
            }
        }
    }
}

//接收线程的任务函数
void *pthread_recv(void *arg) {
    int index;
    char buf[DATA_SIZE];
    char data[DATA_SIZE - sizeof("客户端11")];
    char send_buf[DATA_SIZE];
    while(1) {
        if(cli_num > 0) {
            for(int i = 0; i < CLIENT_MAX_NUM; i++) {
                if(clifd[i] != 0) {
                    index = i + 1;      //客户端索引为数组下标加一
                    ssize_t size = recv(clifd[i], buf, sizeof(buf), MSG_DONTWAIT);
                    if(size > 0) {
                        system("date '+%Y-%m-%d %H:%M:%S'");
                        printf("客户端%d:%s\n", index, buf);   

                        int recv_index = Sep_index_data(buf, data);
                        sprintf(send_buf, "客户端%d:%s", index, data);
                        if(recv_index == 0) {
                            Server_Send(recv_index, send_buf, clifd);
                        }
                        else if(recv_index > 0 && recv_index <= CLIENT_MAX_NUM && clifd[index-1] != 0) {
                            Server_Send(recv_index, send_buf, clifd); 
                        }
                        else {
                            printf("格式错误,不进行转发\n");
                        }
                    }
                    else if(size == 0) {    //size 为 0 表示对应客户端断开连接
                        clifd[i] = 0;       //清空客户都套接字
                        cli_num--;          //更新客户都在线数量
                        system("date '+%Y-%m-%d %H:%M:%S'");    
                        printf("客户端%d退出\n", index);
                    }
                }
            }
        }
    }
}

int main() {
    // 创建一个 IPv4 的 TCP 套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1) {
        perror("socket failed");
        return -1;
    }

    //初始化服务器地址结构体
    SA_IN ser_addr;
    bzero(&ser_addr, sizeof(SA_IN));    //清零
    ser_addr.sin_family = AF_INET;           //IPv4
    ser_addr.sin_port = htons(PORT_NUM);        //将端口号转换位网络字节序赋给端口字段
    ser_addr.sin_addr.s_addr = inet_addr(SERVER_IP);   //将字符串的 IP 地址转换位网络字节序赋给地址字段

    //设置地址允许重复使用
    int reuse = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    int ret = bind(sockfd, (SA *)&ser_addr, sizeof(SA_IN));
    if(ret == -1) {
        perror("bind failed");
        return -1;
    }

    printf("服务器开启,正在监听\n");
    
    ret = listen(sockfd, CLIENT_MAX_NUM);
    if(ret == -1) {
        perror("listen failed");
        return -1;
    }

    //创建线程
    pthread_t tid_send, tid_recv;
    pthread_create(&tid_send, NULL, pthread_send, NULL);
    pthread_create(&tid_recv, NULL, pthread_recv, NULL);

    //循环检测是否有客户端连接
    int i;      //客户端编号索引
    SA_IN cliaddr[CLIENT_MAX_NUM];
    socklen_t cliaddr_len = sizeof(cliaddr);
    char buf[128];
    while(1) {
        //遍历寻找为连接的客户端编号的索引
        for(i = 0; i < CLIENT_MAX_NUM; i++) {
            if(clifd[i] == 0)   break;      //如果寻找到未连接的编号 i 则退出循环进行连接
        }
        if(i >= CLIENT_MAX_NUM)     continue;   //如果没有找到则继续循环  

        clifd[i] =  accept(sockfd, (SA *)&cliaddr, &cliaddr_len);
        if(clifd[i] == -1) {
            perror("accept failed");
            return -1;
        }
        printf("成功连接客户端%d\n", i+1);
        sprintf(buf, "你是客户端%d", i+1);
        send(clifd[i], buf, sizeof(buf), 0);
        
        cli_num++;              //更新客户端在线数量
        if(cli_num == CLIENT_MAX_NUM) {
            printf("客户端数量达到上限\n");
        }
    }

    pthread_join(tid_send, NULL);
    pthread_join(tid_recv, NULL);

}