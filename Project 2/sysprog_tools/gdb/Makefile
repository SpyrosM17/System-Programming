CC=gcc
CFLAGS=-std=c99 -Wall -O0 -D_XOPEN_SOURCE -g -ggdb

all: mmul_gdb

mmul_gdb: mmul_gdb.c
	$(CC) $(CFLAGS) -o mmul_gdb mmul_gdb.c

clean:
	rm -f mmul_gdb

