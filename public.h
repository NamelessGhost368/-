#ifndef _PUBLIC_H
#define _PUBLIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>


#define SERVER_IP   "192.168.10.191"   //服务器 IP 地址
#define PORT_NUM    5000               //服务器端口号
#define CLIENT_MAX_NUM  3
#define DATA_SIZE   256

typedef struct sockaddr SA;         //IPv6
typedef struct sockaddr_in SA_IN;   //IPv4

int Sep_index_data(char *source, char *send_buf);
void Server_Send(int index, char *data, int clifd[CLIENT_MAX_NUM]);

#endif
