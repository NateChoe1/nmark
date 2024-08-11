#ifndef HAVE_MAGIC
#define HAVE_MAGIC

/* TODO: this file is kind of bloated */

#include "limits.h"

enum node {
	NODE_OL,
	NODE_UL,
	NODE_LI,
	NODE_H1,
	NODE_H2,
	NODE_P,
	NODE_BLOCKQUOTE,
	NODE_CODE_BLOCK,
	NODE_START_PARAGRAPH,

	NODE_LEAF_SEPARATOR,

	LEAF_PARAGRAPH,
	LEAF_ESCAPED,
	LEAF_RAW,
	LEAF_EMPTY_LINE, /* pseudo-leaf, corresponds with empty lines and
			    comments, ignored by nmark */
	NODE_NIL /* invalid node, doesn't correspond with anything */
};

struct nodestack {
	enum node nodes[MAX_NODES]; /* only contains nodes, not leaves */
	int len;
	int file_index;

	int is_paragraph;
	enum node curr_type;
};

struct line {
	char *orig_text;
	char *orig_file;
	long line_num;

	/* lines from argv[0] = 0, argv[1] = 1, etc. used to detect file
	 * transitions */
	int file_index;

	/* these only contain nodes, not leaves */
	enum node popped_nodes[MAX_NODES];
	enum node new_nodes[MAX_NODES];
	int num_popped_nodes;
	int num_new_nodes;

	enum node type; /* must be a leaf */
	char *data;

	/* vestigial data that's specific to each leaf type. */
	void *leaf_data;
};

/* populates popped_nodes, new_nodes, num_popped_nodes, num_new_nodes, type, and
 * data.
 *
 * returns error code */
extern int findnodes(struct nodestack *stack, struct line *line);

/* generates open/close html tags for a given node type */
extern char *open_node(enum node node);
extern char *close_node(enum node node);

extern void reset_stack(struct nodestack *stack);

#endif
