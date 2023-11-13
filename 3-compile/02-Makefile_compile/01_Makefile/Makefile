
# 默认执行第一条指令
test : main.c sub.c sub.h
	gcc -o test main.c sub.c
	




# $ ls
# main.c  Makefile  sub.c  sub.h
# 
# 编译文件
# $ make
# gcc -o test main.c sub.c
# 
# 生成 test 目标文件
# $ ls
# main.c  Makefile  sub.c  sub.h  test
# 
# 修改文件更新时间，可以重新编译
# $ make
# make: 'test' is up to date.
# $ touch main.c
# $ make
# gcc -o test main.c sub.c
