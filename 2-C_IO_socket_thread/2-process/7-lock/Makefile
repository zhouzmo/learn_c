# 使用变量定义源文件和目标文件
SOURCE_SRC := 2-demo.c
SOURCE_SRC2 := sem_util.c
SOURCE_OUT := demo.out



# 默认目标，即直接运行`make`命令时的操作
all: $(SOURCE_OUT)

$(SOURCE_OUT): $(SOURCE_SRC)
	gcc -o $(SOURCE_OUT) $(SOURCE_SRC) $(SOURCE_SRC2)


# 清理生成的可执行文件
clean:
	rm -f *.out