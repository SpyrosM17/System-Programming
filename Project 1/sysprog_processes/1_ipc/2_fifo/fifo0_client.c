// fifo0_client.c
#include "fifo.h"

int main()
{
	int readfd , writefd ;

	/* Open the FIFOs. We assume the server has already created them */
	if ((writefd = open(FIFO1, O_WRONLY )) < 0)
	printf("client: can't open write fifo : %s", FIFO1);

	if ((readfd = open(FIFO2, O_RDONLY )) < 0)
		printf("client: can't open read fifo : %s", FIFO2);


	// start of client code
  double x[2];
  double y[2];

  read(readfd, x, 2*sizeof(double));

  printf("client: x[0]=%f, x[1]=%f\n", x[0], x[1]);
  y[0] = x[0] + 100;
  y[1] = x[1] + 100;

  write(writefd, y, 2*sizeof(double));
	// end of client code

	close(readfd);
	close(writefd);

	/* Delete the FIFOs, now that we're finished. */
	if (unlink(FIFO1) < 0)
		printf("client: can't unlink %s", FIFO1);
	if (unlink(FIFO2) < 0)
		printf("client: can't unlink %s", FIFO2);

	return 0;
}
