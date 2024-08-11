#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "nmark.h"
#include "nodes.h"
#include "print.h"
#include "errors.h"

static int process_line(char *path, int file_idx, long line_num, char *text,
		struct nodestack *stack, FILE *out);

static int read_lines(char *path, int idx, struct nodestack *stack, FILE *out);
static char *read_line(FILE *file);

int runjob(int argc, char **argv) {
	long i;
	int res;
	FILE *out;
	struct nodestack stack;

	out = fopen(argv[argc-1], "w");
	if (out == NULL) {
		return NMARK_ERR_OPEN_FAIL;
	}

	reset_stack(&stack);
	for (i = 0; i < argc-1; ++i) {
		if ((res = read_lines(argv[i], i, &stack, out)) != 0) {
			goto end;
		}
	}

	/* hack to make sure that every node is escaped at the end */
	if ((res = process_line("nmark internal file", -1, -1, "<", &stack,
					out))
			!= 0) {
		goto end;
	}

	res = NMARK_ERR_SUCCESS;
end:
	fclose(out);
	return res;
}

static int process_line(char *path, int file_idx, long line_num, char *text,
		struct nodestack *stack, FILE *out) {
	struct line line;
	int res;
	int i;

	line.orig_text = text;
	line.orig_file = path;
	line.line_num = line_num;
	line.file_index = file_idx;

	if ((res = findnodes(stack, &line)) != 0) {
		fprintf(stderr, "%s:%ld: %s\n", line.orig_file, line.line_num,
				err2str(res));
		return res;
	}

	for (i = line.num_popped_nodes - 1; i >= 0; --i) {
		fputs(close_node(line.popped_nodes[i]), out);
	}
	for (i = 0; i < line.num_new_nodes; ++i) {
		fputs(open_node(line.new_nodes[i]), out);
	}
	print_data(&line, out);
	return 0;
}

/* TODO: this function has a subtle dependency on process_line. ideally this
 * function would take in a callback for each line, but C doesn't have closures
 * so that's not easy.*/
static int read_lines(char *path, int idx, struct nodestack *stack, FILE *out) {
	FILE *file;
	long linenum;
	int res;

	file = fopen(path, "r");
	for (linenum = 1;; ++linenum) {
		char *line;
		line = read_line(file);
		if (line == NULL) {
			break;
		}
		if ((res = process_line(path, idx, linenum, line, stack, out))
				!= 0) {
			goto end;
		}
	}
	res = 0;
end:
	fclose(file);
	return res;
}

static char *read_line(FILE *file) {
	char *line_buffer;
	size_t line_len, line_alloc;

	line_alloc = 80;
	line_buffer = xmalloc(line_alloc);
	line_len = 0;
	for (;;) {
		int ch = fgetc(file);

		if (ch == EOF && line_len == 0) {
			free(line_buffer);
			return NULL;
		}

		if (line_len >= line_alloc) {
			line_alloc = line_len * 2;
			line_buffer = xrealloc(line_buffer, line_alloc);
		}

		if (ch == EOF || ch == '\n') {
			line_buffer[line_len] = '\0';
			return line_buffer;
		}
		line_buffer[line_len++] = ch;
	}
}
