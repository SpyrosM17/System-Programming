CC = gcc
CFLAGS = -Wall

all: fifo0_client fifo0_server fifo1

fifo0_client: fifo0_client.c fifo.h
	$(CC) $(CFLAGS) -o $@ $<

fifo0_server: fifo0_server.c fifo.h
	$(CC) $(CFLAGS) -o $@ $<

fifo1: fifo1.c
	$(CC) $(CFLAGS) -o $@ $<


clean:
	rm -f fifo0_client fifo0_server fifo1
