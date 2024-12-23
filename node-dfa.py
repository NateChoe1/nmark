#!/usr/bin/env python3

# This code is kind of confusing, but I've structured it so that you only have
# to worry about this dictionary.

nodedict = {
    "NODE_OL": "1",
    "NODE_UL": "*",
    "NODE_LI": "-",
    "NODE_H1": "#1",
    "NODE_H2": "#2",
    "NODE_H3": "#3",
    "NODE_H4": "#4",
    "NODE_H5": "#5",
    "NODE_H6": "#6",
    "NODE_P": ".",
    "NODE_BLOCKQUOTE": ">",
    "NODE_CODE_BLOCK": "```",
    "NODE_START_PARAGRAPH": "p",
    "LEAF_PARAGRAPH": "|",
    "LEAF_ESCAPED": '"',
    "LEAF_RAW": '<',
    "LEAF_SH_RAW": "$<",
    "LEAF_SH_COOKED": "$$",
    "LEAF_SH_DATA": "$|",
    "LEAF_EMPTY_LINE": '//',
}

# You can safely ignore everything below this line.

import re

def escapestr(string):
    return re.sub(r'([\\"])', r'\\\1', string)

def escapechr(char):
    if char == '\n':
        return "\\n"
    if char == '"':
        return "\\\""
    if char == "\'":
        return "\\'"
    return char

print(
"""
#include <stddef.h>

#include "nodes.h"
#include "node-dfa.h"
char *node2str(enum node node) {
    switch (node) {
""")

for node in nodedict:
    print("case %s: return \"%s\";" % (node, escapestr(nodedict[node])))

print(
"""
    default: return "";
    }
}
""")

print(
"""
int nodelen(enum node node) {
    switch (node) {
""")

for node in nodedict:
    print("case %s: return %d;" % (node, len(nodedict[node])))

print(
"""
    default: return 0;
    }
}
""")

trie = {}
for node in nodedict:
    text = nodedict[node]
    ptr = trie
    for char in text[:-1]:
        if char in ptr and not type(ptr[char]) is dict:
            exit(1)
        if not char in ptr:
            ptr[char] = {}
        ptr = ptr[char]
    ptr[text[-1]] = node

print("char *str2node(char *str, enum node *ret) {")

def print_case(root, level):
    print("switch (str[%d]) {" % (level))
    for char in root:
        if type(root[char]) is str:
            print("case '%s': *ret = %s; return str + %d;" % (escapechr(char), root[char], level+1))
        else:
            print("case '%s':" % (escapechr(char)))
            print_case(root[char], level+1)
    print("""
default: *ret = NODE_NIL; return NULL;
}
""")
print_case(trie, 0)

print("}")
