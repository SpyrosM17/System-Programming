#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
	int sz;

	int fd = open("foo.bin", O_RDONLY);
	if (fd < 0)
	{
		perror("r1");
		exit(1);
	}

	double a = 0;
	double b = 0;

	sz = read(fd, &a, sizeof(double));
	printf("read %d bytes for a\n", sz);
	sz = read(fd, &b, sizeof(double));
	printf("read %d bytes for b\n", sz);

	printf("a=%f, b=%f\n", a, b);

	close(fd);
	return 0;
}
