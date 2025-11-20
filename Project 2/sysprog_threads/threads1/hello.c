// Hello world from a POSIX thread
#include <stdio.h>
#include <pthread.h>


void *work(void *arg)
{
	pthread_t me = pthread_self();
	printf("Hello world from thread %lx!\n", (long)me);

	return NULL;
}

int main(int argc, char **argv)
{
	long i = 1;
	pthread_t thread;

	printf("Hello world from primary thread %lx!\n", (long)pthread_self());

	pthread_create(&thread, NULL, work,(void *) i);
	pthread_join(thread, NULL);

	printf("Child thread ended, exiting!\n");

	return 0;
}
