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

	if (pid == 0) {
		printf ("child before: a=%d, b=%d\n", a, b);
		a++; b++;
		printf ("child after: a=%d, b=%d\n", a, b);
	}
	else {
		printf ("parentbefore: a=%d, b=%d\n", a, b);
		a--; b--;
		printf ("parent after: a=%d, b=%d\n", a, b);
	}

	return (0);
}
