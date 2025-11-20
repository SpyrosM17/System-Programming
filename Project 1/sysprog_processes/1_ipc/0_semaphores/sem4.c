#include <unistd.h>
#include <stdio.h>			/* printf() */
#include <stdlib.h>			/* exit(), malloc(), free() */
#include <sys/types.h>	/* key_t, sem_t, pid_t */
#include <sys/shm.h>		/* shmat(), IPC_RMID */
#include <errno.h>			/* errno, ECHILD */
#include <semaphore.h>	/* sem_open(), sem_destroy(), sem_wait().. */
#include <fcntl.h>			/* O_CREAT, O_EXEC */

void display(char *str);


/* TODO: declare semaphores */

int main ()
{
  int i;						/* loop variables */
  pid_t pid[4];			/* fork pid */
  int child_status;

  /* Clear screen */
  system("clear");

  /* TODO: create and initialize semaphores */

  /********************************************************/
  /* fork child processes                                 */
  /********************************************************/
  for (i=0; i<4; i++)
  {
    pid[i] = fork ();

    if (pid[i] == 0)
    {
      break;
    }
  }

  /* 1st child */
  if (pid[0] == 0)
  {
     /* TODO: synchronize (if required) */

     printf ("My brothers have finished\n\n");  // system ("cat file");

     /* TODO: synchronize (if required) */
   }
   /* 2nd child */
   else if (pid[0] != 0 && pid[1] == 0)
   {
     /* TODO: synchronize (if required) */

     printf ("Hello world\n\n");  // system("echo Hello world > file");

     /* TODO: synchronize (if required) */
   }
   /* 3rd child */
   else if (pid[0] != 0 && pid[1] != 0 && pid[2] == 0)
   {
     /* TODO: synchronize (if required) */

	   printf ("Have a nice day\n\n");  // system("echo Have a nice day >> file");

     /* TODO: synchronize (if required) */
   }
   /* 4th child */
   else if (pid[0] != 0 && pid[1] != 0 && pid[2] != 0 && pid[3] == 0)
   {
     /* TODO: synchronize (if required) */

	   printf ("Mr Perfect\n\n");  // system("echo Mr Perfect >> file");

     /* TODO: synchronize (if required) */
   }
   else
   {
      for (i = 0; i < 4; i++)
      {
        pid_t wpid = waitpid(pid[i], &child_status, 0);
        if (WIFEXITED(child_status))
        {
          printf("Child: %d terminated with exit status %d\n", wpid, WEXITSTATUS(child_status));
        }
        else
        {
          printf("Child: %d terminate abnormally\n", wpid);
        }
      }

      printf(" Parent unlinks the semaphores\n Bye\n");

      /* TODO: unlink and close the semaphores */
  }

  return 0;
}
