#include <ctype.h>

#include "print.h"

static void print_style(char *data, long i, char chr, char *open, char *close,
		FILE *out);
static void print_paragraph(struct line *line, FILE *out);
static void print_escaped(struct line *line, FILE *out);
static void print_escape_char(char ch, FILE *out);

void print_data(struct line *line, FILE *out) {
	if (line->type != LEAF_EMPTY_LINE && line->num_new_nodes == 0 &&
			line->num_popped_nodes == 0) {
		fputc('\n', out);
	}

	switch (line->type) {
	case LEAF_PARAGRAPH:
		print_paragraph(line, out);
		return;
	case LEAF_ESCAPED:
		print_escaped(line, out);
		return;
	case LEAF_RAW:
		fputs(line->data, out);
		return;
	case LEAF_SET_VAR:
		return;
	case LEAF_USE_VAR:
		fputs((char *) line->leaf_data, out);
		return;
	default: return;
	}
}

static void print_style(char *data, long i, char chr, char *open, char *close,
		FILE *out) {
	int open_left, open_right;
	open_left = (i == 0) || (!isalnum(data[i-1]));
	open_right = (data[i+1] == '\0') || (!isalnum(data[i+1]));
	switch (open_left << 1 | open_right) {
	case 2:
		fputs(open, out);
		break;
	case 1:
		fputs(close, out);
		break;
	default:
		fputc(chr, out);
		break;
	}
}

static void print_paragraph(struct line *line, FILE *out) {
	long i;
	char *data;

	data = line->data;
	for (i = 0; data[i] != '\0'; ++i) {
		switch (data[i]) {
#define STYLE(chr, open, close) \
		case chr: \
			print_style(data, i, chr, open, close, out); \
			break
		STYLE('/', "<i>", "</i>");
		STYLE('*', "<b>", "</b>");
		STYLE('_', "<u>", "</u>");
#undef STYLE
		case '\\':
			if (data[i+1] == '\0') {
				fputc('\\', out);
				break;
			}

			/* we /don't/ do things like &amp; replacement with
			 * escaped chars. */
			fputc(data[++i], out);
			break;
		default:
			print_escape_char(data[i], out);
			break;
		}
	}
}

static void print_escaped(struct line *line, FILE *out) {
	long i;
	char *data;
	
	data = line->data;
	for (i = 0; data[i] != '\0'; ++i){
		print_escape_char(data[i], out);
	}
}

static void print_escape_char(char ch, FILE *out) {
	switch (ch) {
	case '&': fputs("&amp;", out); return;
	case ';': fputs("&semi;", out); return;
	case '<': fputs("&lt;", out); return;
	case '>': fputs("&gt;", out); return;
	default: fputc(ch, out); return;
	}
}
