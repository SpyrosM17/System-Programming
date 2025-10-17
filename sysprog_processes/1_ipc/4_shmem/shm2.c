#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE  8

void worker(int id, float *shm_vec)
{
		shm_vec[id] = getpid();
}

int main()
{
    int i, pid;
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
    for (i = 0; i <SIZE; i++) shm_vec[i] = 0;

		for (i = 1; i < SIZE; i++)		// i = 1 ? 
		{
				pid = fork();
				if (pid == 0)
				{
					worker(i, shm_vec);
					exit(0);
				}
		}
		worker(0, shm_vec);

    while ((pid = waitpid(-1, NULL, 0)) > 0);

		for (i = 0; i <SIZE; i++) {
			printf("parent: shm_vec[%d] = %f\n", i, shm_vec[i]);
    }

    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);

		return 0;
}
