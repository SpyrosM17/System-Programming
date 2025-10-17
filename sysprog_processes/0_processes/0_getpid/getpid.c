// gcc -Wall -o getpid getpid.c
#include <stdio.h>	/* for printf */
#include <unistd.h> /* for getpid */

int main()
{
	printf("Process id: %d\n", getpid());
	return 0;
}
