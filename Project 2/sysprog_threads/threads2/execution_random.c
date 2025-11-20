// Spawning 4 threads
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define N 10

void *routine(void *arg)
{
  int id = *((int *) arg);

  printf("Hello from thread:%d\n", id);

  return 0;
}


int main() {
  int i;
  pthread_t t[N];
  int a[N];

  for (i = 0; i < N; i++) {
    a[i] = i;
    pthread_create(&t[i], NULL, routine, (void *)&a[i]);
  }

  for (i = 0; i < N; i++)
    pthread_join(t[i], NULL); 

  return 0;
}

