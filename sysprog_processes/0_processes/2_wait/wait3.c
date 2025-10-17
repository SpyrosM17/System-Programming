#include <stdio.h>
#include <stdlib.h> /* For exit() */
#include <unistd.h> /* For fork(), getpid () */
#include <sys/wait.h> /* For waitpid () */

void child()
{
	sleep(5);
	exit(5);	/* child exits with 5 */
}

int main() {
	int pid, status;

	pid = fork();
	if (pid == 0) { /* child */
		child();
	}
	printf("parent (%d) waits for child (%d)\n", getpid(), pid);
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		printf("child exited normally with value %d\n", WEXITSTATUS(status));
	else
		printf("child was terminated abnormally\n");
	return 0;
}

