// Passing an argument in the wrong way causes race condition
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void *func(void *arg)
{
	long id = *(long *)arg;

	printf("Hello world from thread %ld!\n", id);
	sleep(id);
	return NULL;
}

int main(int argc, char * argv[])
{
	pthread_t id[4];

/*
	for (long i = 0; i < 4; i++)   {
		pthread_create(&id[i], NULL, func, (void *) &i);
	}
*/

    long i;
    i = 0;
	pthread_create(&id[i], NULL, func, (void *) &i);
    i = 1;
	pthread_create(&id[i], NULL, func, (void *) &i);
    i = 2;
	pthread_create(&id[i], NULL, func, (void *) &i);
    i = 3;
	pthread_create(&id[i], NULL, func, (void *) &i);
    i = 4;

	for (long i = 0; i < 4; i++)   {
		pthread_join(id[i], NULL);
	}

	return 0;
}
