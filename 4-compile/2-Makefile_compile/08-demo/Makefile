
# CROSS_COMPILE ?= arm-buildroot-linux-gnueabihf-
CROSS_COMPILE =
CC = $(CROSS_COMPILE)gcc
CFLAGS = -O2 -Wall
SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj

# 查找源文件并生成目标文件列表
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# 默认目标，将这里的 myprogram 替换为您希望的目标文件名
TARGET = demo.out

# 生成可执行文件
$(TARGET): $(OBJS) $(OBJ_DIR)/main.o
	$(CC) $(CFLAGS) -o $@ $^


# 编译 .c 文件为 .o 文件
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# 创建目录
.PHONY: $(OBJ_DIR)
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR) 

# 清理中间文件和可执行文件
clean:
	rm -rf $(OBJ_DIR) 

.PHONY: all clean

