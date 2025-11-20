//fork2.c
#include <stdio.h>
#include <unistd.h>

int main()
{
	int pid;

  printf("parent (%d) about to call fork\n", getpid());
	pid = fork();

	if (pid == 0)
	{
		printf ("I am the child process, pid=%d, parent pid=%d\n", getpid(), getppid());
	}
	else
	{
		sleep(2);
		printf ("I am the parent process, pid=%d, child pid=%d\n", getpid(), pid);
	}

	return (0);
}
