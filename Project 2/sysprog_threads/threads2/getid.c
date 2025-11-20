// Threads are self assigned a unique number
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t id_mutex = PTHREAD_MUTEX_INITIALIZER;
long id_temp = 0;


void *func(void *arg)
{
    int id;
    pthread_mutex_lock(&id_mutex);
    id = id_temp++;
    pthread_mutex_unlock(&id_mutex);


    pthread_mutex_lock(&print_mutex);
	printf("Hello world from thread %d!\n", id);
    pthread_mutex_unlock(&print_mutex);
	sleep(id);
    return NULL;
}

int main(int argc, char * argv[])
{
	pthread_t id[4];

	for (long i = 0; i < 4; i++)   {
		pthread_create(&id[i], NULL, func, NULL);
	}

	for (long i = 0; i < 4; i++)   {
		pthread_join(id[i], NULL);
	}

	return 0;
}
