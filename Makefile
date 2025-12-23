OBJ=errors.o  main.o  nmark.o  nodes.o  print.o  util.o node-dfa.o
HEADERS=$(wildcard *.h)
CFLAGS=-O2 -ansi -Wall -Wpedantic -Wextra -Werror -ggdb -D_POSIX_C_SOURCE
LDFLAGS=
OUT=nmark
PREFIX=/usr

$(OUT): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $< -c -o $@

node-dfa.c: node-dfa.py
	./$< > $@

install: $(OUT)
	cp $(OUT) $(PREFIX)/bin/$(OUT)

uninstall:
	rm $(PREFIX)/bin/$(OUT)

clean:
	rm -f *.o nmark

.INTERMEDIATE: node-dfa.c
.PHONY: install uninstall clean
