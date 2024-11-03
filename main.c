#include <stdio.h>
#include <signal.h>

#include "nmark.h"
#include "consts.h"

static int pline();

int main(int argc, char **argv) {
	if (argc > 1) {
		return runjob(argc-1, argv+1);
	}

	signal(SIGCHLD, SIG_IGN);

	for (;;) {
		int res;
		if ((res = pline()) != 0) {
			return res - 1;
		}
	}
}

static int pline() {
	char line[MAX_LINE];
	char *argv[MAX_ARGS];
	int argc;
	int i;
	int res;

	argv[0] = line;
	argc = 1;
	for (i = 0; i < MAX_LINE; ++i) {
		line[i] = getchar();
		switch (line[i]) {
		default:
			continue;
		case '\n':
			line[i] = '\0';
			goto gotargs;
		case '\0':
			if (argc > MAX_ARGS) {
				goto oom;
			}
			argv[argc++] = line + i + 1;
			break;
		case EOF:
			return 1;
		}
	}
oom:
	return 2;

gotargs:
	res = runjob(argc, argv);
	if (res == 0) {
		return 0;
	}
	return res+1;
}
