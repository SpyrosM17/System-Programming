#include <unistd.h>
#include <stdio.h>			/* printf() */
#include <stdlib.h>			/* exit(), malloc(), free() */
#include <sys/types.h>	/* key_t, sem_t, pid_t */
#include <sys/shm.h>		/* shmat(), IPC_RMID */
#include <errno.h>			/* errno, ECHILD */
#include <semaphore.h>	/* sem_open(), sem_destroy(), sem_wait().. */
#include <fcntl.h>			/* O_CREAT, O_EXEC */

void display(char *str);

/* semaphores are usually declared global variables */
/* synch semaphores */
sem_t *synch1;
sem_t *synch2;
sem_t *synch3;
sem_t *synch4;

int main ()
{
  int i;						/* loop variables */
  pid_t pid[4];			/* fork pid */
  int child_status;

  /* Clear screen */
  system("clear");

  /********************************************************/
  /* initialize semaphores for shared processes           */
  /********************************************************/
  synch1 = sem_open ("Sem1", O_CREAT | O_EXCL, 0644, 0);
  synch2 = sem_open ("Sem2", O_CREAT | O_EXCL, 0644, 0);
  synch3 = sem_open ("Sem3", O_CREAT | O_EXCL, 0644, 0);
  synch4 = sem_open ("Sem4", O_CREAT | O_EXCL, 0644, 0);


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
     sem_wait (synch3);       /* UP operation */
     sem_wait (synch4);       /* UP operation */

     printf ("My brothers have finished\n\n");
     /* system ("cat file"); */
   }
   /* 2nd child */
   else if (pid[0] != 0 && pid[1] == 0)
   {
     /* system("echo Hello world > file"); */
     printf ("Hello world\n\n");
     sem_post (synch1);       /* DOWN operation */
     sem_post (synch2);       /* DOWN operation */
   }
   /* 3rd child */
   else if (pid[0] != 0 && pid[1] != 0 && pid[2] == 0)
   {
     sem_wait (synch1);       /* UP operation */
	   printf ("Have a nice day\n\n");
     /* system("echo Have a nice day >> file"); */
     sem_post (synch3);       /* DOWN operation */
   }
   /* 4th child */
   else if (pid[0] != 0 && pid[1] != 0 && pid[2] != 0 && pid[3] == 0)
   {
     sem_wait (synch2);       /* UP operation */
	   printf ("Mr Perfect\n\n");
	   /*system("echo Mr Perfect >> file");*/
     sem_post (synch4);       /* DOWN operation */
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

      /* unlink prevents the semaphore existing forever */
      /* if a crash occurs during the execution         */
      sem_unlink ("Sem1");
      sem_close(synch1);
      sem_unlink ("Sem2");
      sem_close(synch2);
      sem_unlink ("Sem3");
      sem_close(synch3);
      sem_unlink ("Sem4");
      sem_close(synch4);
  }

  return 0;
}
