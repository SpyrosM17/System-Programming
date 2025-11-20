// deadlock due to bad order of locking
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mut1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut2 = PTHREAD_MUTEX_INITIALIZER;

void *thread0(void *arg)
{
	printf("Hello from thread0\n");

	pthread_mutex_lock(&mut1);
	sleep(1);
	pthread_mutex_lock(&mut2);

	pthread_mutex_unlock(&mut2);
	pthread_mutex_unlock(&mut1);

	printf("Goodbye from thread0\n");

	return NULL;
}

void *thread1(void *arg)
{
	printf("Hello from thread1\n");

	pthread_mutex_lock(&mut2);
	sleep(1);
	pthread_mutex_lock(&mut1);

	pthread_mutex_unlock(&mut1);
	pthread_mutex_unlock(&mut2);

	printf("Goodbye from thread1\n");

	return NULL;
}

int main(int argc, char **argv)
{
	pthread_t tid0, tid1;

	printf("Hello from main()\n");

	pthread_create(&tid0, NULL, thread0, NULL);
	pthread_create(&tid1, NULL, thread1, NULL);

	pthread_join(tid0, NULL);
	pthread_join(tid1, NULL);

	printf("Goodbye from main()\n");
	return 0;
}
