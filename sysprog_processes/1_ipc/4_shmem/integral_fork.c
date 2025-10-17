// numerical integration: parallel version with processes and shared memory
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>

double get_wtime(void)
{
  struct timeval t;

  gettimeofday(&t, NULL);

  return (double)t.tv_sec + (double)t.tv_usec*1.0e-6;
}

double f(double x)
{
  return log(x)*sqrt(x);
}


#define SIZE  2	// number of workers

void worker(int id, float *shm_vec)
{
	double a = 1.0;
	double b = 4.0;
	unsigned long const n = 1e9;
	const double dx = (b-a)/n;

	double S = 0;

	for (unsigned long i = id; i < n; i+=SIZE) {
		double xi = a + (i + 0.5)*dx;
		S += f(xi);
	}
	S *= dx;
	shm_vec[id] = S;
}

int main()
{
    int i, pid;
    int shmid;
    key_t key;
    char *shm;
    float *shm_vec;

    /*
     * We'll name our shared memory segment
     * "5678".
     */
    key = 5678;

    /*
     * Create the segment.
     */
    if ((shmid = shmget(key, SIZE*sizeof(float), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    shm_vec = (float *) shm;
    for (i = 0; i <SIZE; i++) shm_vec[i] = 0;


		double t0 = get_wtime();
		for (i = 0; i < SIZE; i++)		// i = 1 ? 
		{
				pid = fork();
				if (pid == 0)
				{
					worker(i, shm_vec);
					exit(0);
				}
		}

    while ((pid = waitpid(-1, NULL, 0)) > 0);

		double total_S = 0;
		for (i = 0; i <SIZE; i++) {
			printf("parent: shm_vec[%d] = %f\n", i, shm_vec[i]);
			total_S += shm_vec[i];
    }
		double t1 = get_wtime();
		printf("Time=%lf seconds, Result=%.8f\n", t1-t0, total_S);

    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);

		return 0;
}
