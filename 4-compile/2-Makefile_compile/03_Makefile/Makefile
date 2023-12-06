
test : main.o sub.o
	gcc -o test main.o sub.o
	
# 通配符
# 缺点：不能监测文件的修改
%.o : %.c
	gcc -c -o $@  $<
	
clean:
	rm *.o test -f
	


# $ ls
# main.c  Makefile  sub.c  sub.h
# $ make
# gcc -c -o main.o  main.c
# gcc -c -o sub.o  sub.c
# gcc -o test main.o sub.o
# $ ./test
# Main fun!
# Sub fun, A= 1!
# 
# 问题：不能监测 .h 文件的修改
# $ cat sub.h
# #define A  1
# void sub_fun(void);
# $ vim sub.h 修改为 A 2
# $ make
# make: 'test' is up to date.
# $ ./test
# Main fun!
# Sub fun, A= 1!


