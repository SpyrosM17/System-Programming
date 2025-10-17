#include <unistd.h>
#include <stdio.h>			/* printf() */
#include <stdlib.h>			/* exit(), malloc(), free() */
#include <sys/types.h>	/* key_t, sem_t, pid_t*/
#include <sys/shm.h>		/* shmat(), IPC_RMID*/
#include <errno.h>			/* errno, ECHILD*/
#include <semaphore.h>	/* sem_open(), sem_destroy(), sem_wait().. */
#include <fcntl.h>			/* O_CREAT, O_EXEC*/
#include <time.h>

int main ()
{
	int i;							/* loop variables*/
	int t;							/* temporary variable*/
	key_t shmkey;				/* shared memory key */
	int shmid;					/* shared memory id*/
	pid_t pid;					/* fork pid*/
	int *p;							/* shared variable */ /*shared */
	unsigned int n; 		/* fork count*/
	int *nSeconds;			/* Dynamic array */
											/* stores the number of secs */
											/* the i-process waits */

	/* Initializes random number generator */
	srand(time(NULL));


	/* Initialize a shared variable in shared memory*/
	/* valid directory name and a number*/
	shmkey = ftok ("/dev/null", 5);
	shmid = shmget (shmkey, sizeof (int), 0644 | IPC_CREAT);

	/* shared memory error check */
	if (shmid < 0)
	{
		perror ("shmget\n");
		exit (1);
	}

	/* attach p to shared memory */
	p= (int *) shmat (shmid, NULL, 0); 
	*p = 0;

	/********************************************************/
	printf ("How many children do you want to fork?\n");
	printf ("Fork count: ");
	scanf("%u", &n);
	/********************************************************/


	/* Dynamic array which stores the number of secs */
	/* the i-process will wait */
	nSeconds = (int *)malloc(n * sizeof(int));

	for (i=0; i<n; i++) nSeconds[i] = rand()%4 + 1;
	/********************************************************/

	/* fork child processes */
	for (i = 0; i < n; i++)
	{
		pid = fork ();
		if (pid < 0) {
			printf ("Fork error.\n");
		}
		else if (pid == 0) /* child processes */
			break;
	}

	/******************************************************/
	/******************** PARENT PROCESS ******************/
	/******************************************************/
	if (pid > 0)
	{
		/* wait for all children to exit */
    while ((pid = waitpid(-1, NULL, 0)) > 0)
		{
			if (errno == ECHILD)
				break;
		}

		printf (" Parent: All children have exited.\n");
		printf (" *p = %d\n\n\n", *p);

		/* shared memory detach */
		shmdt (p);
		shmctl (shmid, IPC_RMID, 0);
	}

	/******************************************************/
	/******************** CHILD PROCESS *******************/
	/******************************************************/
	else
	{
		printf (" Child(%d) enters the critical section.\n", i);
		printf (" Waiting %d seconds.\n", nSeconds[i]);
		t = *p;
		t++;
		sleep (nSeconds[i]);
		*p = t;
		printf (" Increasing *p\n");
		printf (" Child(%d) exits the critical region\n", i);
		printf (" new value of *p=%d.\n\n\n", *p);
	}

	return 0;
}
