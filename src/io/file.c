#include "file.h"

#include "debug/assert.h"

FILE *file_open(const char *path)
{
	FILE *file = fopen(path, "rb");
	HE_ASSERT(file != NULL, "Failed to open the file");
	return file;
}

void file_close(FILE *file)
{
	fclose(file);
}

void file_read(FILE *file, size_t *size, char *dest)
{
	HE_ASSERT(file != NULL, "Cannot read NULL");

	fseek(file, 0, SEEK_END);
	long pos_end = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (size != NULL) *size = (size_t)pos_end;

	if (dest != NULL)
	{
		fread(dest, sizeof(char), pos_end, file);
	}
}
