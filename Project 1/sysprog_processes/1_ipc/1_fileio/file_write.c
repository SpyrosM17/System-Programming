#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
  int sz;

  int fd = open("foo.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
	{
		perror("r1");
		exit(1);
	}

	double a = 33.3;
	double b = 66.6;
	sz = write(fd, &a, sizeof(double));
	printf("wrote %d bytes for a\n", sz);
	sz = write(fd, &b, sizeof(double));
	printf("wrote %d bytes for b\n", sz);

	close(fd);
	return 0;
}
