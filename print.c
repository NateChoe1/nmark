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
	case LEAF_EMPTY_LINE: default:
		return;
	}
}

void copy_file(FILE *in, FILE *out, int escape_entities) {
	int c;
	for (;;) {
		if ((c = fgetc(in)) == EOF) {
			return;
		}
		if (!escape_entities) {
			fputc(c, out);
			continue;
		}
		switch (c) {
		case '&':
			fputs("&amp;", out);
			break;
		case ';':
			fputs("&semi;", out);
			break;
		case '<':
			fputs("&lt;", out);
			break;
		case '>':
			fputs("&gt;", out);
			break;
		case '\'':
			fputs("&apos;", out);
			break;
		case '"':
			fputs("&quot;", out);
			break;
		default:
			fputc(c, out);
			break;
		}
	}
}

static void print_style(char *data, long i, char chr, char *open, char *close,
		FILE *out) {
	int open_left, open_right;
	long j;

	open_left = 1;
	for (j = i-1; j >= 0 && !isspace(data[j]); --j) {
		if (isalnum(data[j])) {
			open_left = 0;
			break;
		}
	}

	open_right = 1;
	for (j = i+1; data[j] != '\0' && !isspace(data[j]); ++j) {
		if (isalnum(data[j])) {
			open_right = 0;
			break;
		}
	}

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
		/* inline styles */
#define STYLE(chr, open, close) \
		case chr: \
			print_style(data, i, chr, open, close, out); \
			break
		STYLE('/', "<i>", "</i>");
		STYLE('*', "<b>", "</b>");
		STYLE('_', "<u>", "</u>");
		STYLE('`', "<code>", "</code>");
#undef STYLE

		/* hyperlinks (basic 2 char string replacements) */
		case '<':
			if (data[i+1] != '(') {
				goto normal;
			}
			++i;
			fputs("<a href='", out);
			break;
		case ')':
			if (data[i+1] != '[') {
				goto normal;
			}
			++i;
			fputs("'>", out);
			break;
		case ']':
			if (data[i+1] != '>') {
				goto normal;
			}
			++i;
			fputs("</a>", out);
			break;

		/* escaped characters */
		case '\\':
			if (data[i+1] == '\0') {
				fputc('\\', out);
				break;
			}

            /* let's say we want to put a single @ sign in a code block. `@`
             * won't work because the the backticks can't detect that they're
             * part of an actual word rather than just standalone characters. to
             * fix this we can insert a nonce letter like `@\0` to indicate that
             * there is an actual word there.
             *
             * similarly, if we want to put text directly next to stylized text,
             * such as when we have many `object`s, we can say `object`\ s
             * */
            if (data[i+1] == '0' || data[i+1] == ' ') {
                ++i;
                break;
            }

			/* we /don't/ do things like &amp; replacement with
			 * escaped chars. */
			fputc(data[++i], out);
			break;

		/* normal characters */
		default: normal:
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
