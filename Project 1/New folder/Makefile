CC=gcc
CFLAGS=-Wall -O3
LDFLAGS=-lm



all: integral_mc_seq integral_mc_fifo integral_mc_mmap integral_mc_mmap_sem integral_mc_mq

integral_mc_seq: integral_mc_seq.c
	$(CC) $(CFLAGS) -o integral_mc_seq integral_mc_seq.c $(LDFLAGS)

	$(CC) $(CFLAGS) -o integral_mc_fifo integral_mc_fifo.c $(LDFLAGS)
	$(CC) $(CFLAGS) -o integral_mc_mmap integral_mc_mmap.c $(LDFLAGS)
	$(CC) $(CFLAGS) -o integral_mc_mmap_sem integral_mc_mmap_sem.c $(LDFLAGS) -lrt
	$(CC) $(CFLAGS) -o integral_mc_mq integral_mc_mq.c $(LDFLAGS) -lrt

clean:
	rm -f integral_mc_seq integral_mc_fifo integral_mc_mmap integral_mc_mmap_sem integral_mc_mq
