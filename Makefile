# 项目名称
TARGET_CLIENT = client
TARGET_SERVER = server

# 源文件路径
SRC_CLIENT = client.c public.c
SRC_SERVER = server.c public.c

# 编译器
CC = gcc

# 编译选项
CFLAGS = -Wall -pthread

# 生成目标
all: $(TARGET_CLIENT) $(TARGET_SERVER)

# 编译客户端
$(TARGET_CLIENT): $(SRC_CLIENT)
	$(CC) $(CFLAGS) $(SRC_CLIENT) -o $(TARGET_CLIENT)

# 编译服务器
$(TARGET_SERVER): $(SRC_SERVER)
	$(CC) $(CFLAGS) $(SRC_SERVER) -o $(TARGET_SERVER)

# 清理目标文件
clean:
	rm -f $(TARGET_CLIENT) $(TARGET_SERVER)


