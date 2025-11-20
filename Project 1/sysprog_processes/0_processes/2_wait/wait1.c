#include <stdio.h>
#include <stdlib.h> /* For exit() */
#include <unistd.h> /* For fork(), getpid () */
#include <sys/wait.h> /* For waitpid () */

int main() {
	int pid;

	pid = fork();
	if (pid == 0) { /* child */
		return 5; /* child exits with 5 */
	}
	printf("parent (%d) waits for child (%d)\n", getpid(), pid);
	waitpid(pid, NULL, 0);
	printf("child terminated\n");
	return 0;
}

