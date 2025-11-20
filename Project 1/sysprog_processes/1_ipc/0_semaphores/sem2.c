#include <stdio.h>			/* printf() */
#include <stdlib.h>			/* exit(), malloc(), free() */
#include <sys/types.h>	/* key_t, sem_t, pid_t*/
#include <errno.h>			/* errno, ECHILD  */
#include <unistd.h>

void display(char *str);

int main ()
{
	int i;			/*loop variables  */
	pid_t pid;	/*fork pid*/

	system("clear");

	/* fork child processes */
	pid = fork ();
  if (pid > 0)
	{
		for (i=0; i<10; i++)
		{
			display ("ab");
		}

		wait(NULL);
	}
	else if (pid == 0)
	{
		for (i=0; i<10; i++)
		{
			display ("cd\n");
		}
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
