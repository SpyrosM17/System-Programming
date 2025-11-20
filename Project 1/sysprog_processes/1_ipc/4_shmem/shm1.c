#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE  8

int main()
{
    int i;
    int shmid;
    key_t key;
    char *shm;
    float *shm_vec;

    /*
     * We'll name our shared memory segment
     * "5678".
     */
    key = 5678;

    /*
     * Create the segment.
     */
    if ((shmid = shmget(key, SIZE*sizeof(float), IPC_CREAT | 0666)) < 0) {
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

    shm_vec = (float *) shm;

    /*
     * Now put some things into the memory for the
     * other process to read.
     */
    shm_vec[0] = -1;
    for (i = 1; i <SIZE; i++)
        shm_vec[i] = (float) i;

    int pid = fork();
    if (pid > 0) {
        /*
         * Finally, we wait until the other process
         * changes the first element of our vector
         * to '-99', indicating that it has read and modified
         * what we put there.
         */

        while (shm_vec[0] == -1)
            sleep(1);

        waitpid(-1, NULL, 0);

        for (i = 0; i <SIZE; i++)
            printf("parent: shm_vec[%d] = %f\n", i, shm_vec[i]);

    }
    else if (pid == 0) {
        /*
         * Now read what the server put in the memory.
         */

        for (i = 0; i <SIZE; i++)
            printf("child: shm_vec[%d] = %f\n", i, shm_vec[i]);

        for (i = 1; i <SIZE; i++)
            shm_vec[i] = shm_vec[i] + 100;

        shm_vec[0] = -99.0;

        exit(0);
    }
    else {
        perror("fork");
    }


    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);

		return 0;
    exit(0);
}
