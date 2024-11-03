#ifndef HAVE_PRINT
#define HAVE_PRINT

#include <stdio.h>

#include "nodes.h"

extern void print_data(struct line *line, FILE *out);
extern void copy_file(FILE *in, FILE *out, int escape_entities);

#endif
