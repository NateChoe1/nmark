#1|nmark: an actual markup language

.p|nmark is a markup language that I hacked together. The goal was to have
nice-looking text that's easy to read, easy to parse, and that doesn't have very
much extra code. For example, the only nmark specific code in this paragraph is
the ".p|" at the beginning.

.|nmark is a line based markup language. This paragraph is written to illustrate
 |that idea. In nmark, the "." string corresponds to the HTML <p> node. The
 |spaces on subsequent lines indicate that the <p> node should not be closed.

.|If we didn't have the spaces at the beginning of each line...
.|This line and the one before it would be on different paragraphs.

.p|The "p" pseudo-node indicates that the current node configuration should be
continued for the rest of the (nmark, not HTML) paragraph. This means that for
subsequent lines, I don't have to define which nodes they're in; nmark will just
assume that it's they're the same as that initial line. This continues until
nmark encounters an empty line.

.p|We can also nest HTML nodes.

>.|This text is inside of a block quote. The ">." corresponds to a
  |<blockquote><p> nested node.

 .|The following example is an unordered list of features in nmark. "*"
  |corresponds to a <ul> tag, and "-" corresponds to a <li> tag.

 *-|Nested nodes
 *-|/italicized/, _underscored_, and *bold* text.

.p|There are two kinds of nodes in nmark: regular nodes (which are often just
called "nodes"), and leaf nodes (which are often just called "leaves"). Nodes
directly correspond with HTML tags and leaves impact the way that text within
nodes is rendered. The paragraph leaf (represented by "|") automatically adds
/italics/, _underscores_, and *bold text*.

.p"In contrast, the escaped leaf (represented by '"') doesn't add /italics/,
_underscores_, or *bold text*. It does, however, automatically escape things
like <special html characters>.

.p<The raw leaf (represented by "&lt;"), is like the escaped leaf, except that
it doesn't escape raw HTML things.

.p|The end goal is for nmark to replace ncdg, but there are a few things that I
still need to add:

*-|Cleaner inline styles (it's not possible to make text that's italicized and
  |bolded).
 -|Hyperlinks
 -|Images
