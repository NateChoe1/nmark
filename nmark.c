#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "vars.h"
#include "nmark.h"
#include "nodes.h"
#include "print.h"
#include "errors.h"

static int populate_nodes(struct linebuf *lines);

static void add_line(struct linebuf *out,
		char *line, char *path, long num, int idx);
static void read_lines(char *path, int idx, struct linebuf *out);
static char *read_line(FILE *file);

int runjob(int argc, char **argv) {
	long i;
	int res;
	FILE *out;
	static struct linebuf file;

	if (file.lines == NULL) {
		file.alloc = 1024;
		file.lines = xmalloc(file.alloc * sizeof *file.lines);
	}
	file.len = 0;

	out = fopen(argv[argc-1], "w");
	if (out == NULL) {
		return NMARK_ERR_OPEN_FAIL;
	}
	for (i = 0; i < argc-1; ++i) {
		read_lines(argv[i], i, &file);
	}

	/* hack to make sure that every node is escaped at the end */
	add_line(&file, "<", "nmark internal file", -1, -1);

	if ((res = populate_nodes(&file)) != NMARK_ERR_SUCCESS) {
		goto end;
	}

	if ((res = fill_vars(&file)) != NMARK_ERR_SUCCESS) {
		goto end;
	}

	for (i = 0; i < (long) file.len; ++i) {
		struct line *line;
		int j;
		line = file.lines + i;
		for (j = line->num_popped_nodes-1; j >= 0; --j) {
			fputs(close_node(line->popped_nodes[j]), out);
		}
		for (j = 0; j < line->num_new_nodes; ++j) {
			fputs(open_node(line->new_nodes[j]), out);
		}
		print_data(line, out);
	}

	res = NMARK_ERR_SUCCESS;
end:
	for (i = 0; (size_t) i < file.len-1; ++i) {
		free(file.lines[i].orig_text);
	}
	fclose(out);
	return res;
}

static int populate_nodes(struct linebuf *lines) {
	size_t i;
	struct nodestack stack;
	reset_stack(&stack);
	for (i = 0; i < lines->len; ++i) {
		struct line *line;
		int res;
		line = lines->lines + i;
		res = findnodes(&stack, line);
		if (res != 0) {
			fprintf(stderr, "%s:%ld: %s\n",
					line->orig_file, line->line_num,
					err2str(res));
			return res;
		}
	}
	return NMARK_ERR_SUCCESS;
}

static void add_line(struct linebuf *out,
		char *line, char *path, long num, int idx) {
	if (out->len >= out->alloc) {
		out->alloc = out->len * 2;
		out->lines = xrealloc(out->lines,
				out->alloc * sizeof *out->lines);
	}
	out->lines[out->len].orig_text = line;
	out->lines[out->len].orig_file = path;
	out->lines[out->len].line_num = num;
	out->lines[out->len].file_index = idx;
	++out->len;
}

static void read_lines(char *path, int idx, struct linebuf *out) {
	FILE *file;
	long linenum;

	file = fopen(path, "r");
	for (linenum = 1;; ++linenum) {
		char *line;
		line = read_line(file);
		if (line == NULL) {
			break;
		}
		add_line(out, line, path, linenum, idx);
	}
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
