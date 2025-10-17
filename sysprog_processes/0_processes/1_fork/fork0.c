#include <stdio.h>
#include <unistd.h> /* Needed for fork(), getpid () */

int main() {
	printf("parent (%d) about to call fork\n", getpid());
	fork();
	printf("Hi from process %d\n", getpid());
	return 0;
}
