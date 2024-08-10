#include <stdio.h>
#include <stdlib.h>

#include "util.h"

void *xmalloc_real(size_t size, char *file, long linenum) {
	void *ret;
	ret = malloc(size);
	if (ret == NULL) {
		fprintf(stderr, "%s:%ld xmalloc(%lu) failed\n",
				file, linenum, size);
		exit(EXIT_FAILURE);
	}
	return ret;
}

void *xrealloc_real(void *ptr, size_t size, char *file, long linenum) {
	void *ret;
	ret = realloc(ptr, size);
	if (ret == NULL) {
		fprintf(stderr, "%s:%ld xrealloc(%p, %lu) failed\n",
				file, linenum, ptr, size);
		exit(EXIT_FAILURE);
	}
	return ret;
}
