// Barrier
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>


#if defined(__APPLE__)
//#warning "Barrier are not supported on OSX, using a custom implementation"
#include "barrier.h"
#include "barrier.c"

#define pthread_barrier_t barrier_t
#define pthread_barrier_init barrier_init
#define pthread_barrier_wait barrier_wait

#endif

pthread_barrier_t bar;

void *func(void *arg)
{
	int id = *((int *)arg);
	printf("Hello world from thread %d!\n", id);

	//pthread_barrier_wait(&bar);
	sleep(id);

	printf("Goodbye world from thread %d!\n", id);

	return NULL;
}

int main(int argc, char * argv[])
{
	pthread_t id[4];
	int a[4];

	pthread_barrier_init(&bar, NULL, 4);

	for (long i = 0; i < 4; i++)   {
		a[i] = i;
		pthread_create(&id[i], NULL, func, &a[i]);
	}

	for (long i = 0; i < 4; i++)   {
		pthread_join(id[i], NULL);
	}

	return 0;
}

