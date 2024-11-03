#include <stdio.h>

#include "errors.h"

char *err2str(int errnum) {
	switch (errnum) {
	case NMARK_ERR_SUCCESS:
		return "Success";
	case NMARK_ERR_BROKEN_SPACE:
		return "Inherited node doesn't consist of only spaces";
	case NMARK_ERR_EXTRA_SPACE:
		return "Node specification has spaces outside of inheritance";
	case NMARK_ERR_UNKNOWN_NODE:
		return "Unknown node type";
	case NMARK_ERR_TOO_DEEP:
		return "Too many nested nodes";
	case NMARK_ERR_INVALID_INPUT:
		return "Invalid input";
	case NMARK_ERR_DUP_VAR:
		return "Duplicate variable declaration";
	case NMARK_ERR_INVALID_VAR:
		return "Use of invalid variable";
	case NMARK_ERR_EXEC_FAIL:
		return "Failed to fork process";
	default:
		return "Unknown error code";
	}
}

void print_error(int errnum, struct line *line, FILE *file) {
	fprintf(file, "%s:%ld: %s\n",
			line->orig_file, line->line_num+1, err2str(errnum));
}
