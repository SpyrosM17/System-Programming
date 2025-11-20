// Spawning 4 threads
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define N 10

pthread_mutex_t m[N];
pthread_cond_t c[N];

void *routine(void *arg)
{
  int id = *((int *) arg);

  if (id > 0) {
    pthread_mutex_lock(&m[id]);
    pthread_cond_wait(&c[id], &m[id]);
  }

  printf("Hello from thread:%d\n", id); 

  if (id > 0) pthread_mutex_unlock(&m[id]);
  if (id < N) pthread_cond_signal(&c[id+1]);

  return 0;
}


int main() {
  int i;
  pthread_t t[N];
  int a[N];

  for (i = 0; i < N; i++) {
     pthread_cond_init(&c[i], NULL);
     pthread_mutex_init(&m[i], NULL);
  }

  for (i = 0; i < N; i++) {
    a[i] = i;
    pthread_create(&t[i], NULL, routine, (void *)&a[i]); 
  }

  for (i = 0; i < N; i++)
    pthread_join(t[i], NULL); 

  return 0;
}

