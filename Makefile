SRC=$(wildcard *.c) node-dfa.c
OBJ=$(SRC:.c=.o)
HEADERS=$(wildcard *.h)
CFLAGS=-O2 -ansi -Wall -Wpedantic -Wextra -Werror -ggdb -D_POSIX_C_SOURCE
LDFLAGS=
OUT=nmark

$(OUT): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $< -c -o $@

node-dfa.c: node-dfa.py
	./$< > $@

.INTERMEDIATE: node-dfa.c
