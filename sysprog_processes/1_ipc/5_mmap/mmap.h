#ifndef __MMAP_H__
#define __MMAP_H__

void *open_and_map(char *fname, int *pfd, int size);
void sync_data(void *map, int fd, int size);
void unmap_and_close(void *map, int fd, int size);
void *create_and_map(char *fname, int *pfd, int size);

#endif

