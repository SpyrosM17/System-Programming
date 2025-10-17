#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>
#include "mmap.h"

int main(int argc, char *argv[])
{
	printf("Hello from process %d\n", getpid());

	int n = 1;
	int bytes = n*sizeof(double);
	int fd;
	double *a;
  double s = 0;

	a = create_and_map("data.bin", &fd, bytes);
	for (int i = 0; i < n; i++)
		a[i] = 0;

	int pid = fork();
  	if (pid > 0) {
    sem_wait()
    a[0] += 33;
    sem_post()
		waitpid(pid, NULL, 0);
		for (int i = 0; i < n; i++)
			printf("a[%d] = %lf\n", i, a[i]);

		for (int i = 0; i < n; i++)
      s += a[i];

    printf("result = %f\n", s);

		unmap_and_close(a, fd, bytes);
		unlink("data.bin");
	} else {
    sem_wait()
    a[0] += 66;
    sem_post();
		unmap_and_close(a, fd, bytes);
		exit(1);
	}

	return 0;
}
