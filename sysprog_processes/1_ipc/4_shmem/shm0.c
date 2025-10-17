#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SHMSZ     27

int main()
{
    char c;
    int shmid;
    key_t key;
    char *shm, *s;

    /*
     * We'll name our shared memory segment
     * "5678".
     */
    key = 5678;

    /*
     * Create the segment.
     */
    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }


    int pid = fork();
    if (pid > 0) {
       /*
        * Now put some things into the memory for the
        * other process to read.
        */
        s = shm;

        for (c = 'a'; c <= 'z'; c++)
            *s++ = c;
        *s = 0; //NULL;

        /*
         * Finally, we wait until the other process 
         * changes the first character of our memory
         * to '*', indicating that it has read what 
         * we put there.
         */

        while (*shm != '*')
            sleep(1);

        waitpid(-1, NULL, 0);
    }
    else if (pid == 0) {
        /*
         * Now read what the server put in the memory.
         */
        for (s = shm; *s != 0; s++)
            putchar(*s);
        putchar('\n');

        /*
         * Finally, change the first character of the 
         * segment to '*', indicating we have read 
         * the segment.
         */
        *shm = '*';

        exit(0);
    }
    else {
        perror("fork");
    }


    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);

    exit(0);
}
