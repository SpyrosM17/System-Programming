#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int workers = 4;

void worker(int i)
{
	printf("I'm one of the children [i: %d,  pid: %d, ppid: %d] of %d\n", i, getpid(), getppid(), workers);
	exit(1);
}

int main(void){
    int i, tmp, res;

    for (i=0; i<workers; i++) {
        res = fork();
        if (res == 0)
            worker(i);
        else
            continue;
    }

    // That's the father, it waits for all the childs
    printf("I'm the father [pid: %d, ppid: %d]\n",getpid(),getppid());
    for(i=0; i<workers; i++) {
        wait(&tmp);
    }

    return 0;
}
