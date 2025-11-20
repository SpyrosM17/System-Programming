// Spawning 4 threads and passing an argument to their function
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

int x = 5;

void *func(void *arg)
{
	long id = (long) *((long *)arg);

	printf("Hello world from thread %ld!\n", id);
	sleep(id);
	x = 6;
	return arg;
}

int main(int argc, char * argv[])
{
	pthread_t id[4];
	long result;
	long a[4];

	for (long i = 0; i < 4; i++)   {
	    a[i] = i;
		pthread_create(&id[i], NULL, func, (void *) &a[i]);
	}

	for (long i = 0; i < 4; i++)   {
		pthread_join(id[i], (void *)&result);
	}

	return 0;
}
