#include "public.h"
#include <string.h>

/**
  * @brief  分离索引和数据的函数
  * @note   从字符串中提取数字索引并将剩余部分的数据复制到 `send_buf`
  *         假设源字符串 `source` 中，索引是由连续数字组成，数据部分是紧跟在索引后面的字符串。
  *         比如：输入 "1234Hello World"，则：
  *         - 索引 `index = 1234`
  *         - 数据部分 `send_buf = "Hello World"`
  * @param  source   输入的源字符串，包含索引和数据
  * @param  send_buf 存放提取出来的数据部分
  * @retval 索引值（整数型）
  */
int Sep_index_data(char *source, char *send_buf) {
    int i = 0;
    int index;
    char str_index[20];

    // 提取索引（数字部分）
    while(source[i] >= '0' && source[i] <= '9') {
        str_index[i] = source[i];
        i++;
    }
    str_index[i] = '\0';  // 结束符

    // 将数据部分复制到 send_buf
    strcpy(send_buf, source + i);

    // 将索引字符串转换为整数
    index = atoi(str_index);  

    return index;
}


/**
  * @brief  服务器发送数据函数（单播/群发）
  * @note   通过客户端文件描述符 `clifd` 发送数据：
  *         - 若 `index == 0`，则群发数据到所有已连接的客户端
  *         - 否则，数据发送到指定索引的客户端（索引从 1 开始）
  * @param  index    客户端索引，`0` 表示群发，其他值表示指定客户端（1 ~ CLIENT_MAX_NUM）
  * @param  data     要发送的数据
  * @param  clifd    客户端文件描述符数组，包含所有已连接客户端的文件描述符
  * @retval None
  */
void Server_Send(int index, char *data, int clifd[CLIENT_MAX_NUM]) {
    if(index == 0) {        //群发
        for(int i = 0; i < CLIENT_MAX_NUM; i ++) {
            if(clifd[i] != 0) {
                send(clifd[i], data, DATA_SIZE,0);
            }
        }
    }
    else {
        int i = index - 1;      //索引转化为数组下标
        send(clifd[i], data, DATA_SIZE,0);   
    }
}