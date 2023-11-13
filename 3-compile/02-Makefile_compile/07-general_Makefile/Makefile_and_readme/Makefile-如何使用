
# ===== step3：指定工具链的前缀 =====
CROSS_COMPILE = 
AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm

STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump

export AS LD CC CPP AR NM
export STRIP OBJCOPY OBJDUMP

# ===== step2：确定编译选项、链接选项 =====
# 修改顶层目录Makefile的CFLAGS，这是编译所有.c文件时都要用的编译选项;
# 修改顶层目录Makefile的LDFLAGS，这是链接最后的应用程序时的链接选项;

CFLAGS := -Wall -O2 -g
CFLAGS += -I $(shell pwd)/include

LDFLAGS := 

export CFLAGS LDFLAGS

TOPDIR := $(shell pwd)
export TOPDIR

# ===== step4：确定应用程序的名字 =====
TARGET := test 	

# ===== step1：确定编译哪些源文件 =====
obj-y += main.o
obj-y += sub.o
obj-y += a/ #需要根据 / 判断是否是目录，不可忽略


all : start_recursive_build $(TARGET)
	@echo $(TARGET) has been built!

start_recursive_build:
	make -C ./ -f $(TOPDIR)/Makefile.build

$(TARGET) : start_recursive_build
	$(CC) -o $(TARGET) built-in.o $(LDFLAGS)

clean:
	rm -f $(shell find -name "*.o")
	rm -f $(TARGET)

distclean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(TARGET)

# 执行"make"来编译，执行"make clean"来清除，执行"make distclean"来彻底清除