// pi computation / multithreaded version
#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>

double get_wtime(void)
{
        struct timeval t;
        gettimeofday(&t, NULL);
        return (double)t.tv_sec + (double)t.tv_usec*1.0e-6;
}


#define NUM_THREADS 2
pthread_t thread[NUM_THREADS];
pthread_mutex_t Mutex;
long num_steps = 1e9;
double step;
double global_sum = 0.0;

void *Pi (void *arg)
{
	long start;
	double x, sum = 0.0;

	start = (long) (*(int *)arg);
	step = 1.0/(double) num_steps;

	for (long i=start; i<num_steps; i+=NUM_THREADS)
	{
		x = (i+0.5)*step;
		sum = sum + 4.0/(1.0+x*x);
	}

	pthread_mutex_lock (&Mutex);
	global_sum += sum;
	pthread_mutex_unlock(&Mutex);

	return 0;
}

int main ()
{
	double pi;

	int Arg[NUM_THREADS];

	for (int i=0; i<NUM_THREADS; i++)   
		Arg[i] = i;

	pthread_mutex_init(&Mutex, NULL);

	double t0 = get_wtime();

	for (int i=0; i<NUM_THREADS; i++)
		pthread_create(&thread[i], NULL, Pi, &Arg[i]);
   
	for (int i=0; i<NUM_THREADS; i++)
		pthread_join(thread[i], NULL);

	pi = global_sum * step;

	double t1 = get_wtime();

	printf("pi = %lf, elapsed time = %lf s\n", pi, t1-t0);
   
	return 0;
}
