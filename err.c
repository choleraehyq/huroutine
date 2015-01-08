#include <stdio.h>
#include <stdlib.h>

void errexit(const char *s) {
	fprintf(stderr, "%s\n", s);
	exit(1);
}
