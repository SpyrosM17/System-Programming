#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define N 5

int main()
{
	int i;
	int pid;

	for (i=0; i<N; i++)
	{
		pid = fork();
		if (pid > 0)
		{
			printf("i = %d Father = %5d, Id = %5d, Child = %5d\n", i, getppid(), getpid(), pid);
			wait(NULL);
			break;
		}
	}

	return (0);
}
