// Need for sychnronization
// How to run with 4 threads (default): ./racecondition2 
// How to specify the number of threads: ./racecondition2 2
// Note: can you predict the output? :-)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NITERS 1000000

long counter = 0;

// This code is extremely simple, so let's disable compiler optimizations (GCC)
#pragma GCC push_options
#pragma GCC optimize ("O0")
void *func(void *arg)
{
	for (long i = 0; i < NITERS; i++)
		counter++;

	return NULL;
}
#pragma GCC pop_options


int main(int argc, char * argv[])
{
	long nthreads = 4;

	if (argc == 2) nthreads = atol(argv[1]);

	pthread_t *id = malloc(nthreads*sizeof(pthread_t));

	for (long i = 0; i < nthreads; i++)   {
		pthread_create(&id[i], NULL, func, NULL);
	}

	for (long i = 0; i < nthreads; i++)   {
		pthread_join(id[i], NULL);
	}

	printf("nthreads = %ld, expected = %ld, counter = %ld\n", nthreads, nthreads*NITERS, counter);

	free(id);

	return 0;
}
