#pragma once

#include <base.h>
#include <stdio.h>

void fread_print_error (FILE * nonnull f, const char * nonnull path);
FILE * nullable try_fopen (const char * nonnull path, const char * nonnull mode);

int try_fread (FILE * nonnull f,
	       const char * nonnull path,
	       void * nonnull dest,
	       size_t nbytes);
