# 使用变量定义源文件和目标文件
SERVER_SRC := 4-server-concurrency.c
CLIENT_SRC := 4-client-concurrency.c
SERVER_OUT := server.out
CLIENT_OUT := client.out

# 默认目标，即直接运行`make`命令时的操作
all: $(SERVER_OUT) $(CLIENT_OUT)

# 编译生成服务器可执行文件
$(SERVER_OUT): $(SERVER_SRC)
	gcc -o $(SERVER_OUT) $(SERVER_SRC)

# 编译生成客户端可执行文件
$(CLIENT_OUT): $(CLIENT_SRC)
	gcc -o $(CLIENT_OUT) $(CLIENT_SRC)

# 清理生成的可执行文件
clean:
	rm -f $(SERVER_OUT) $(CLIENT_OUT)