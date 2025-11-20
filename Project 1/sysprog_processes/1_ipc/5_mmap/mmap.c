#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "mmap.h"

void *create_and_map(char *fname, int *pfd, int size)
{
	int fd;
	void *map;
	int result;

	fd = open(fname, O_RDWR | O_CREAT | O_TRUNC | O_SYNC, (mode_t)0600);
	if (fd == -1) {
		perror("Error opening file for writing");
		exit(EXIT_FAILURE);
	}

	/* Stretch the file size to the size of the (mmapped) array */
	result = lseek(fd, size-1, SEEK_SET);
	if (result == -1) {
		close(fd);
		perror("Error calling lseek() to 'stretch' the file");
		exit(EXIT_FAILURE);
	}

	result = write(fd, "", 1);
	if (result != 1) {
		close(fd);
		perror("Error writing last byte of the file");
		exit(EXIT_FAILURE);
	}

	map = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (map == MAP_FAILED) {
		close(fd);
		perror("Error mmapping the file");
		exit(EXIT_FAILURE);
	}

	*pfd = fd;

	return map;
}

void sync_data(void *map, int fd, int size)
{
	msync(map, size, MS_SYNC);
	fsync(fd);
}

void unmap_and_close(void *map, int fd, int size)
{
	if (munmap(map, size) == -1) {
		perror("Error un-mmapping the file");
	}

	close(fd);
}

void *open_and_map(char *fname, int *pfd, int size)
{
	int fd;
	void *map;  /* mmapped array of int's */

	fd = open(fname, O_RDWR | O_SYNC);
	if (fd == -1) {
		perror("Error opening file for reading");
		exit(EXIT_FAILURE);
	}

	map = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (map == MAP_FAILED) {
		close(fd);
		perror("Error mmapping the file");
		exit(EXIT_FAILURE);
	}

	*pfd = fd;
	return map;
}
