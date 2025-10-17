#include <stdio.h>  /* printf()     */
#include <stdlib.h>     /* exit(), malloc(), free() */
#include <sys/types.h>    /* key_t, sem_t, pid_t    */
#include <errno.h>  /* errno, ECHILD  */
#include <unistd.h>

void display(char *str);

int main (int argc, char **argv)
{
	pid_t pid;  /* fork pid */

	/* Clear screen */
	system("clear");

	/* fork child processes */
	pid = fork ();
	if (pid > 0) {
		display ("Good morning ");
		display ("a nice day\n\n");
		wait(NULL);
	}
	else if (pid == 0)
	{
		display ("sir\n");
		display ("I wish you, ");
	}
	else /* fork failed */
	{
		printf ("Fork error.\n");
	}
	return 0;
}

void display(char *str)
{
	char *p;

	for (p=str; *p; p++)
	{
		write(1, p, 1);
		usleep(100);
	}
}
