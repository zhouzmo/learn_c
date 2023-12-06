
test : main.o sub.o
	gcc -o test main.o sub.o
	


%.o : %.c
	gcc -c -o $@  $<

# 增加规则，使得 sub.o 依赖于 sub.c 和 sub.h
sub.o : sub.h


clean:
	rm *.o test -f



