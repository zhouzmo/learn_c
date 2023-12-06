# 使用变量定义源文件和目标文件
SERVER_SRC := thread_gdb.c
SERVER_OUT := a.out

# 默认目标，即直接运行`make`命令时的操作
all: $(SERVER_OUT)

# 编译生成服务器可执行文件
$(SERVER_OUT): $(SERVER_SRC)
	gcc -g -pthread -o $(SERVER_OUT) $(SERVER_SRC)



# 清理生成的可执行文件
clean:
	rm -f *.out