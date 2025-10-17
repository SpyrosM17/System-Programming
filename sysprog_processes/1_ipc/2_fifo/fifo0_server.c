// fifo0_server.c
#include "fifo.h"

int main()
{
	int readfd , writefd;

	/* Create the FIFOs one for reading and one for writing. */
	if (mkfifo(FIFO1, PERMS) < 0)
		printf("can't create fifo : %s", FIFO1);

	if (mkfifo(FIFO2, PERMS) < 0) {
		unlink(FIFO1);
		printf("can't create fifo : %s", FIFO2);
	}

	/* Open the FIFOs one for reading and one for writing */
	if ((readfd = open(FIFO1, O_RDONLY)) < 0) /* A */
		printf("server: can't open read fifo : %s", FIFO1);

	if ((writefd = open(FIFO2, O_WRONLY)) < 0) /* B */
		printf("server: can't open write fifo : %s", FIFO2);

	// start of server code
  double x[2] = {33, 44};
  double y[2];

  write(writefd, x, 2*sizeof(double));
  read(readfd, y, 2*sizeof(double));

  printf("server: y[0]=%f, y[1]=%f\n", y[0], y[1]);
  // end of server code

	close(readfd);
	close(writefd);

	return 0;
}
