CC = gcc
CFLAGS = -Wall -Wextra -g -pthread

all: client.out server.out

client.out: client_demo.c poolsocket.c socketlog.c socketutil.c
	$(CC) $(CFLAGS) $^ -o $@

server.out: server_demo.c poolsocket.c socketlog.c socketutil.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f client.out server.out

.PHONY: all clean