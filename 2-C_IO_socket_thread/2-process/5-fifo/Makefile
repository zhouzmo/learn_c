# 使用变量定义源文件和目标文件
SOURCE_SRC := 1-fifo_read.c
SOURCE_OUT := read.out

SOURCE_SRC2 := 2-fifo_write.c
SOURCE_OUT2 := write.out

# 默认目标，即直接运行`make`命令时的操作
all: $(SOURCE_OUT) $(SOURCE_OUT2)

# 编译生成服务器可执行文件
$(SOURCE_OUT): $(SOURCE_SRC)
	gcc -o $(SOURCE_OUT) $(SOURCE_SRC)

# 编译生成客户端可执行文件
$(SOURCE_OUT2): $(SOURCE_SRC2)
	gcc -o $(SOURCE_OUT2) $(SOURCE_SRC2)

# 清理生成的可执行文件
clean:
	rm -f *.out