//fork3.c
#include <stdio.h>
#include <unistd.h>

int a = 5;

int main()
{
	int b = 10;
	int pid;

  printf("parent (%d) about to call fork\n", getpid());
	pid = fork();

	if (pid == 0)
		printf ("I am the child process, pid=%d, a=%d, b=%d\n", getpid(), a, b);
	else
		printf ("I am the parent process, pid=%d, a=%d, b=%d\n", getpid(), a, b);

	return (0);
}
