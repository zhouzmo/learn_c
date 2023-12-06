# 使用变量定义源文件和目标文件
SERVER_SRC := server.c
CLIENT_SRC := client.c
UTIL_SRC := scr_util.c
SERVER_OUT := server.out
CLIENT_OUT := client.out

# 默认目标，即直接运行 `make` 命令时的操作
all: $(SERVER_OUT) $(CLIENT_OUT)

# 编译生成服务器可执行文件
$(SERVER_OUT): $(SERVER_SRC) $(UTIL_SRC)
	gcc -g -o $(SERVER_OUT) $(SERVER_SRC) $(UTIL_SRC)

# 编译生成客户端可执行文件
$(CLIENT_OUT): $(CLIENT_SRC) $(UTIL_SRC)
	gcc -g -o   $(CLIENT_OUT) $(CLIENT_SRC) $(UTIL_SRC)

# 清理生成的可执行文件
clean:
	rm -f $(SERVER_OUT) $(CLIENT_OUT)