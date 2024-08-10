#ifndef HAVE_ERRORS
#define HAVE_ERRORS

#include <stdio.h>

#include "nodes.h"

#define NMARK_ERR_SUCCESS 0
#define NMARK_ERR_BROKEN_SPACE 1
#define NMARK_ERR_EXTRA_SPACE 2
#define NMARK_ERR_UNKNOWN_NODE 3
#define NMARK_ERR_TOO_DEEP 4
#define NMARK_ERR_OPEN_FAIL 5
#define NMARK_ERR_INVALID_INPUT 6
#define NMARK_ERR_DUP_VAR 7
#define NMARK_ERR_INVALID_VAR 8

extern char *err2str(int errnum);
extern void print_error(int errnum, struct line *line, FILE *file);

#endif
