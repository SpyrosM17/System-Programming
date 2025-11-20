// Spawning 4 threads
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "timer.c"

void *func(void *arg)
{
	pthread_t me = pthread_self();
	printf("Hello world from thread %lx!\n", (long)me);

	sleep(1);
	return NULL;
}

int main(int argc, char * argv[])
{
	pthread_t id[4];

    double t0 = get_wtime();
	for (long i = 0; i < 4; i++)   {
		pthread_create(&id[i], NULL, func, NULL);
	}

	for (long i = 0; i < 4; i++)   {
		pthread_join(id[i], NULL);
	}
    double t1 = get_wtime();
    printf("elapsed time=%lf s\n", t1-t0);

	return 0;
}
