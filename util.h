#ifndef HAVE_UTIL
#define HAVE_UTIL

#include <stdio.h>

#define xmalloc(size) xmalloc_real(size, __FILE__, __LINE__)
#define xrealloc(ptr, size) xrealloc_real(ptr, size, __FILE__, __LINE__)
extern void *xmalloc_real(size_t size, char *file, long linenum);
extern void *xrealloc_real(void *ptr, size_t size, char *file, long linenum);
#endif
