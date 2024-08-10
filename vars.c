#include <limits.h>
#include <stdlib.h>

#include "vars.h"
#include "util.h"
#include "errors.h"

/* number of children in each trie node */
#define NUM_CHARS (UCHAR_MAX + 1)

struct trie_node {
	char *val;
	struct trie_node **children;
};

static int add_var(struct trie_node **trie, char *var);
static struct trie_node *new_trie_node();
static void free_trie(struct trie_node *trie);
static char *get_var(struct trie_node *trie, char *data);

int fill_vars(struct linebuf *buf) {
	struct trie_node *trie;
	size_t i;
	int res;

	trie = NULL;
	for (i = 0; i < buf->len; ++i) {
		if (buf->lines[i].type != LEAF_SET_VAR) {
			continue;
		}
		if ((res = add_var(&trie, buf->lines[i].data))
				!= NMARK_ERR_SUCCESS) {
			goto error;
		}
	}

	for (i = 0; i < buf->len; ++i) {
		char *value;
		if (buf->lines[i].type != LEAF_USE_VAR) {
			continue;
		}
		value = get_var(trie, buf->lines[i].data);
		if (value == NULL) {
			res = NMARK_ERR_INVALID_VAR;
			goto error;
		}
		buf->lines[i].leaf_data = (void *) value;
	}
	free_trie(trie);
	return NMARK_ERR_SUCCESS;
error:
	print_error(res, &buf->lines[i], stderr);
	free_trie(trie);
	return res;
}

static int add_var(struct trie_node **trie, char *data) {
	while (*data != '=') {
		if (*data < 0) {
			return NMARK_ERR_INVALID_INPUT;
		}
		if (*trie == NULL) {
			*trie = new_trie_node();
		}
		trie = (*trie)->children + *data;
		++data;
	}
	if (*trie == NULL) {
		*trie = new_trie_node();
	}
	if ((*trie)->val != NULL) {
		return NMARK_ERR_DUP_VAR;
	}
	(*trie)->val = data + 1;
	return NMARK_ERR_SUCCESS;
}

static struct trie_node *new_trie_node() {
	struct trie_node *ret;
	int i;
	ret = xmalloc(sizeof *ret);
	ret->val = NULL;
	ret->children = xmalloc(NUM_CHARS * sizeof *ret->children);
	for (i = 0; i < NUM_CHARS; ++i) {
		ret->children[i] = NULL;
	}
	return ret;
}

static void free_trie(struct trie_node *trie) {
	int i;

	if (trie == NULL) {
		return;
	}
	for (i = 0; i < NUM_CHARS; ++i) {
		free_trie(trie->children[i]);
	}
	free(trie->children);
	free(trie);
}

static char *get_var(struct trie_node *trie, char *data) {
	if (trie == NULL) {
		return NULL;
	}
	if (data[0] < 0) {
		return NULL;
	}
	if (data[0] == '\0') {
		return trie->val;
	}
	return get_var(trie->children[(int) data[0]], data + 1);
}
