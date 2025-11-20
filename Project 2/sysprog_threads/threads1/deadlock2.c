// deadlock due to recursive locking
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>


int main(int argc, char **argv)
{
	pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

	printf("Hello from main()\n");

	pthread_mutex_lock(&mut);
	pthread_mutex_lock(&mut);

	pthread_mutex_unlock(&mut);
	pthread_mutex_unlock(&mut);

	printf("Goodbye from main()\n");
	return 0;
}
