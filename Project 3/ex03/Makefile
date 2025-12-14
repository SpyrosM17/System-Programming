CC=gcc
CFLAGS=-Wall -O3 -fopenmp #Προσθήκη για compilation openmp
LDFLAGS=-lm

all: ex03a_seq ex03b_seq ex03a_omp ex03b_omp

ex03a_seq: ex03a_seq.c
	$(CC) $(CFLAGS) -o ex03a_seq ex03a_seq.c $(LDFLAGS)

ex03b_seq: ex03b_seq.c
	$(CC) $(CFLAGS) -o ex03b_seq ex03b_seq.c $(LDFLAGS)

ex03a_omp: ex03a_omp.c
	$(CC) $(CFLAGS) -o ex03a_omp ex03a_omp.c $(LDFLAGS)

ex03b_omp: ex03b_omp.c
	$(CC) $(CFLAGS) -o ex03b_omp ex03b_omp.c $(LDFLAGS)

clean:
	rm -f ex03a_seq ex03b_seq ex03a_omp ex03b_omp

