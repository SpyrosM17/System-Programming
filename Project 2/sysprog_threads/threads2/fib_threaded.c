/* Fibonacci: multithreaded, version 4*/
#include <stdio.h>
#include <pthread.h>


int nthreads = 0;
pthread_mutex_t nthreads_mutex = PTHREAD_MUTEX_INITIALIZER;

struct fib_arg {
  int n;
  int res;
};

void *fib(void *arg) {
    struct fib_arg *a = (struct fib_arg *) arg;
    struct fib_arg a1, a2;
    pthread_t t1, t2;

    pthread_mutex_lock(&nthreads_mutex);
    nthreads++;
    pthread_mutex_unlock(&nthreads_mutex);

	if (a->n <= 1) {
       a->res = a->n;
    }
	else {
		a1.n = a->n-1;
		a2.n = a->n-2;
		pthread_create(&t1, NULL, fib, (void *) &a1);
		pthread_create(&t2, NULL, fib, (void *) &a2);
		pthread_join(t1, NULL);
		pthread_join(t2, NULL);
		a->res = a1.res + a2.res;
	}
	return NULL;
}



int main(int argc, char * argv[])
{
    struct fib_arg arg;

    arg.n = 10;

    fib(&arg);

    printf("fib(%d)=%d\n", arg.n, arg.res);
    printf("total number of threads=%d\n", nthreads);

	return 0;
}
