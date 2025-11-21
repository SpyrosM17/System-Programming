CC=gcc
CFLAGS=-Wall -O3 -pthread # Προσθηκη flag: -pthread για μεταγλώτιση
LDFLAGS=-lm

all: ex02_seq ex02_mc1 ex02_mc2

ex02_seq: ex02_seq.c
	$(CC) $(CFLAGS) -o ex02_seq ex02_seq.c $(LDFLAGS)

ex02_mc1: ex02_mc1.c
	$(CC) $(CFLAGS) -o ex02_mc1 ex02_mc1.c $(LDFLAGS)

ex02_mc2: ex02_mc2.c
	$(CC) $(CFLAGS) -o ex02_mc2 ex02_mc2.c $(LDFLAGS)


clean:
	rm -f ex02_seq ex02_mc1 ex02_mc2
