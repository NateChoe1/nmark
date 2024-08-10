#include <string.h>

#include "nodes.h"
#include "errors.h"
#include "node-dfa.h"

/* finds the number of nodes and characters shared (inherited) with (from) the
 * previous line */
static int find_common_nodes(char *line, struct nodestack *stack,
		int *common_nodes, int *common_chars);

/* returns 0 for nodes, 1 for leaves */
static int is_leaf(enum node node);

int findnodes(struct nodestack *stack, struct line *line) {
	int common_nodes, common_chars;
	int res;
	char *rem;
	int starts_paragraph;

	rem = line->orig_text;

	if (rem[0] == '\0') {
		stack->is_paragraph = 0;
		line->num_popped_nodes = 0;
		line->num_new_nodes = 0;
		line->type = LEAF_EMPTY_LINE;
		return NMARK_ERR_SUCCESS;
	}

	if (stack->file_index != line->file_index) {
		stack->is_paragraph = 0;
		stack->file_index = line->file_index;
	}

	if (stack->is_paragraph) {
		line->num_popped_nodes = 0;
		line->num_new_nodes = 0;
		line->type = stack->curr_type;
		line->data = line->orig_text;
		return NMARK_ERR_SUCCESS;
	}

	if ((res = find_common_nodes(rem, stack, &common_nodes, &common_chars))
			!= NMARK_ERR_SUCCESS) {
		return res;
	}

	memcpy(line->popped_nodes, stack->nodes + common_nodes,
			(stack->len - common_nodes) *
			sizeof *line->popped_nodes);
	line->num_popped_nodes = stack->len - common_nodes;

	line->num_new_nodes = 0;

	starts_paragraph = 0;
	rem += common_chars;
	for (;;) {
		enum node this_node;
		char *new_rem;
		if (rem[0] == ' ') {
			return NMARK_ERR_EXTRA_SPACE;
		}
		new_rem = str2node(rem, &this_node);
		if (new_rem == NULL) {
			return NMARK_ERR_UNKNOWN_NODE;
		}
		if (this_node == NODE_START_PARAGRAPH) {
			starts_paragraph = 1;
			goto next_token;
		}
		if (is_leaf(this_node)) {
			line->type = this_node;
			line->data = new_rem;
			break;
		}

		if (line->num_new_nodes >= MAX_NODES) {
			return NMARK_ERR_TOO_DEEP;
		}
		line->new_nodes[line->num_new_nodes++] = this_node;
next_token:
		rem = new_rem;
	}
	if (common_nodes + line->num_new_nodes > MAX_NODES) {
		return NMARK_ERR_TOO_DEEP;
	}
	if (starts_paragraph) {
		stack->is_paragraph = 1;
		stack->curr_type = line->type;
	}
	memcpy(stack->nodes + common_nodes, line->new_nodes,
			line->num_new_nodes * sizeof *line->new_nodes);
	stack->len = common_nodes + line->num_new_nodes;
	return 0;
}

static int find_common_nodes(char *line, struct nodestack *stack,
		int *common_nodes, int *common_chars) {
	*common_nodes = *common_chars = 0;
	for (;;) {
		int i, this_node_len;

		/* every node is in common */
		if (*common_nodes >= stack->len) {
			return 0;
		}

		/* this node deviates from the previous line */
		if (line[*common_chars] != ' ') {
			return 0;
		}

		/* check for broken inherited nodes */
		this_node_len = nodelen(stack->nodes[*common_nodes]);
		for (i = 0; i < this_node_len; ++i) {
			if (line[*common_chars + i] != ' ') {
				return NMARK_ERR_BROKEN_SPACE;
			}
		}

		*common_chars += this_node_len;
		++*common_nodes;
	}

}

static int is_leaf(enum node node) {
	return node > NODE_LEAF_SEPARATOR;
}

char *open_node(enum node node) {
	switch (node) {
	case NODE_OL: return "<ol>";
	case NODE_UL: return "<ul>";
	case NODE_LI: return "<li>";
	case NODE_H1: return "<h1>";
	case NODE_H2: return "<h2>";
	case NODE_P: return "<p>";
	case NODE_BLOCKQUOTE: return "<blockquote>";
	case NODE_CODE_BLOCK: return "<pre><code>";
	default: return "";
	}
}

char *close_node(enum node node) {
	switch (node) {
	case NODE_OL: return "</ol>";
	case NODE_UL: return "</ul>";
	case NODE_LI: return "</li>";
	case NODE_H1: return "</h1>";
	case NODE_H2: return "</h2>";
	case NODE_P: return "</p>";
	case NODE_BLOCKQUOTE: return "</blockquote>";
	case NODE_CODE_BLOCK: return "</code></pre>";
	default: return "";
	}
}

void reset_stack(struct nodestack *stack) {
	stack->len = 0;
	stack->file_index = -1;
	stack->is_paragraph = 0;
}
