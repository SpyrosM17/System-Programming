// pi computation / sequential code
#include <stdio.h>
#include <sys/time.h>

double get_wtime(void)
{
        struct timeval t;
        gettimeofday(&t, NULL);
        return (double)t.tv_sec + (double)t.tv_usec*1.0e-6;
}


long num_steps = 1e9;
double step;

int main()
{  	  
	double x, pi, sum = 0.0;


	double t0 = get_wtime();

	step = 1.0/(double) num_steps;
	for (int i=0; i <num_steps; i++){
		x = (i-0.5)*step;
		sum = sum + 4.0/(1.0+x*x);
	}

	pi = step * sum;

	double t1 = get_wtime();

	printf("pi = %lf, elapsed time = %lf s\n", pi, t1-t0);
	return 0;
}

