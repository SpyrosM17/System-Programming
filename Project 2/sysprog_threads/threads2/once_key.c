// pthread_once + thread private data
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_key_t key;
pthread_once_t key_once = PTHREAD_ONCE_INIT;

void once_routine(void)
{
	pthread_key_create(&key, NULL);
}

void routine()
{
	int *value;
	value = pthread_getspecific(key);
    printf("hello from thread %d\n", *value);
}

void *func(void *arg)
{
    int id = *((int *)arg);

	int *value;
	pthread_once(&key_once, once_routine);

	value = malloc(sizeof(int));
	*value = id;
	pthread_setspecific(key, value);

	routine();

    return NULL;
}


int main(int argc, char * argv[])
{
	pthread_t id[4];
	int a[4];

	for (long i = 0; i < 4; i++)   {
		a[i] = i;
		pthread_create(&id[i], NULL, func, (void *)&a[i]);
	}

	for (long i = 0; i < 4; i++)   {
		pthread_join(id[i], NULL);
	}

	return 0;
}
