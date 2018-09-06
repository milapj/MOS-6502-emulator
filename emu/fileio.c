#include <fileio.h>

#include <errno.h>
#include <string.h>

void
fread_print_error (FILE * f, const char * path)
{
	if (feof(f)) {
		ERROR_PRINT("Unexpected EOF while reading %s", path);
		return;
	}
	ERROR_PRINT("Error reading %s: %s", path, strerror(errno));
}

int
try_fread (FILE * f, const char * path, void * dest, size_t nbytes)
{
	if (fread(dest, nbytes, 1, f) != 1) {
		fread_print_error(f, path);
		return -1;
	}
	return 0;
}

FILE *
try_fopen (const char * path, const char * mode)
{
	FILE * f = fopen(path, mode);
	if (f) {
		return f;
	}

	ERROR_PRINT("Error opening %s: %s", path, strerror(errno));
	return NULL;
}
