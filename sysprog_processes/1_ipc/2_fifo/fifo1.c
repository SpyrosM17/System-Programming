// C program to implement one side of FIFO
// This side writes first, then reads
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


void worker(int i, char *myfifo)
{
	printf("Hello from worker %d [pid=%d]\n", i, getpid());

  // Open FIFO for write only
  int fd = open(myfifo, O_WRONLY);

  // Write the input arr2ing on FIFO
  // and close it
  int pid = getpid();
  write(fd, &pid, sizeof(int));
  close(fd);

  exit(1);
}

int main()
{
  int fd;

  // FIFO file path

  // Write requests of {PIPE_BUF} bytes or less shall not be interleaved
  // with data from other processes doing writes on the same pipe.
  // https://unix.stackexchange.com/questions/68146/what-are-guarantees-for-concurrent-writes-into-a-named-pipe
  char * myfifo = "/tmp/myfifo";

  // Creating the named file(FIFO)
  // mkfifo(<pathname>, <permission>)
  mkfifo(myfifo, 0666);

  for (int i = 0; i < 4; i++)
  {
    int pid = fork();
    if (pid == 0)
      worker(i, myfifo);
  }

  printf("Hello from server %d\n", getpid());

  // Open FIFO for Read only
  fd = open(myfifo, O_RDONLY);

  for (int i = 0; i < 4; i++)
  {
    int res;

    // Read from FIFO
    read(fd, &res, sizeof(res));

    // Print the read message
    printf("server received: %d\n", res);
  }
  close(fd);


  for (int i = 0; i < 4; i++)
  {
    int res = waitpid(-1, NULL, 0);
    printf("res=%d\n", res);
  }



  return 0;
}
