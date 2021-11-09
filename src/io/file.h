#ifndef __io_file_h__
#define __io_file_h__

#include <stddef.h>
#include <stdio.h>

FILE *file_open(const char *path);
void file_close(FILE *file);

void file_read(FILE *file, size_t *size, char *dest);

#endif /* __io_file_h__ */
