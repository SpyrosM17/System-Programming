#include <stdio.h>/* printf() */
#include <stdlib.h> /* exit(), malloc(), free() */
#include <sys/types.h>/* key_t, sem_t, pid_t*/
#include <sys/shm.h>/* shmat(), IPC_RMID*/
#include <errno.h>/* errno, ECHILD*/
#include <semaphore.h>/* sem_open(), sem_destroy(), sem_wait().. */
#include <fcntl.h>/* O_CREAT, O_EXEC*/
#include <unistd.h>

void display(char *str);

/* semaphores are usually declared global variables */
/* synch semaphore 									*/
sem_t *synch1;
sem_t *synch2;

int main (int argc, char **argv)
{
	pid_t pid;/* fork pid */

	/* Clear screen */
	system("clear");

	/* initialize semaphores for shared processes */
	synch1 = sem_open ("Sem1", O_CREAT | O_EXCL, 0644, 0);
	synch2 = sem_open ("Sem2", O_CREAT | O_EXCL, 0644, 0);

	/* fork child processes */
	pid = fork ();
	if (pid > 0) {
		display ("Good morning ");
		sem_post (synch1);/* UP operation */

		sem_wait (synch2);/* DOWN operation */
		display ("a nice day\n\n");
		wait(NULL);

		/* unlink prevents the semaphore existing forever */
		/* if a crash occurs during the execution */
		sem_unlink ("Sem1");
		sem_close(synch1);
		sem_unlink ("Sem2");
		sem_close(synch2);
	}
	else if (pid == 0) {
		sem_wait (synch1);/* DOWN operation */
		display ("sir\n");

		display ("I wish you, ");
		sem_post (synch2);/* UP operation */
	}
	else { /* fork failed */
		/* unlink prevents the semaphore existing forever */
		/* if a crash occurs during the execution */
		sem_unlink ("Sem1");
		sem_close(synch1);
		sem_unlink ("Sem2");
		sem_close(synch2);

		printf ("Fork error.\n");
	}

	return 0;
}

void display(char *str)
{
	char *p;

	for (p=str; *p; p++) {
		write(1, p, 1);
		usleep(100);
	}
}
