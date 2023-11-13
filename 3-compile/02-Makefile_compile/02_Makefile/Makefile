
# test 依赖于 main.o sub.o 
# 链接 main.o sub.o
test : main.o sub.o
	gcc -o test main.o sub.o
	
# -c 对目标文件进行 预处理、编译、汇编 但是不链接
main.o : main.c
	gcc -c -o main.o  main.c

sub.o : sub.c
	gcc -c -o sub.o  sub.c	
	
clean:
	rm *.o test -f


# $ ls
# main.c  Makefile  sub.c  sub.h
# 
# 未指定目标，默认执行第一个目标 test
# 编译文件，生成 .o 和 test 文件
# $ make
# gcc -c -o main.o  main.c
# gcc -c -o sub.o  sub.c
# gcc -o test main.o sub.o
# 
# 修改文件更新时间，可以重新编译
# $ make
# make: 'test' is up to date.
# $ touch sub.c
# $ make
# gcc -c -o sub.o  sub.c 
# gcc -o test main.o sub.o
# $ ls
# main.c  main.o  Makefile  sub.c  sub.h  sub.o  test
# 
# 删除文件
# $ make clean
# rm *.o test -f
# $ ls
# main.c  Makefile  sub.c  sub.h
# $
