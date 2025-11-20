#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void){
    int i, tmp;
    pid_t pid[4];

    for (i=0;i<4;i++) {
        pid[i] = fork();
        if (pid[i] == 0) {
            break;
        }
    }

    if (pid[0] != 0 && pid[1] != 0 && pid[2] != 0 && pid[3] != 0) {
        // That's the father, it waits for all the childs
        printf("I'm the father [pid: %d, ppid: %d]\n",getpid(),getppid());
        for(i=0;i<4;i++) {
            wait(&tmp);
        }
    } else {
        // That's the child, it print its pid, its parent pid and exits.
        printf("I'm one of the children [i: %d,  pid: %d, ppid: %d]\n", i, getpid(), getppid());
    }

    return 0;
}
