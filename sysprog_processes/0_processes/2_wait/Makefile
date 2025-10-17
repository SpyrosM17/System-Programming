# Makefile: the short way
CC=gcc
CFLAGS=-Wall

all: wait1 wait2 wait3
.PHONY:all


%.o: %.c
	$(CC) $(CFLAGS) -o $$ $<

clean:
	rm -f wait1 wait2 wait3
.PHONY:clean
