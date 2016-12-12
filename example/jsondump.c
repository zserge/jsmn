#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "../jsmn.h"

/* Function realloc_it() is a wrapper function for standart realloc()
 * with one difference - it frees old memory pointer in case of realloc
 * failure. Thus, DO NOT use old data pointer in anyway after call to
 * realloc_it(). If your code has some kind of fallback algorithm if
 * memory can't be re-allocated - use standart realloc() instead.
 */
static inline void *realloc_it(void *ptrmem, size_t size) {
	void *p = realloc(ptrmem, size);
	if (!p)  {
		free (ptrmem);
		fprintf(stderr, "realloc(): errno=%d\n", errno);
	}
	return p;
}

/*
 * An example of reading JSON from stdin and printing its content to stdout.
 * The output looks like YAML, but I'm not sure if it's really compatible.
 */

#define INDENT(n) { int k; for (k = 0; k < n; k++) printf("  "); }

static void dump(const char *js, jsmntok_t *t, int indent) {
        jsmntok_t *c;

        if(t->skip < 1) {
            printf("Error: skip = %d: %.*s\n", t->skip, FMT_STR(js, t));
            exit(1);
        }
	if (t->type == JSMN_PRIMITIVE) {
		printf("%.*s", FMT_STR(js, t));
		return;
	} else if (t->type == JSMN_STRING) {
		printf("'%.*s'", FMT_STR(js, t));
		return;
	} else if (t->type == JSMN_OBJECT) {
                if(t->skip == 1) {
                    printf("{}");
                    return;
                }
		printf("{\n");
		for (c = t+1; c < t+t->skip; c += c->skip) {
                        INDENT(indent+1);
			dump(js, c, indent+1);
			printf(": ");
                        if(c->skip > 1) {
                            dump(js, c+1, indent+1);
                        }
			printf(",\n");
		}
                INDENT(indent);
                printf("}");
		return;
	} else if (t->type == JSMN_ARRAY) {
                if(t->skip == 1) {
                    printf("[]");
                    return;
                }
		printf("[\n");
		for (c = t+1; c < t+t->skip; c += c->skip) {
			INDENT(indent+1);
			dump(js, c, indent+1);
			printf(",\n");
		}
                INDENT(indent);
                printf("]");
		return;
	}
	return;
}

int main() {
	int r;
	int eof_expected = 0;
	char *js = NULL;
	size_t jslen = 0;
	char buf[BUFSIZ];

	jsmn_parser p;
	jsmntok_t *tok;
	size_t tokcount = 2;

	/* Prepare parser */
	jsmn_init(&p);

	/* Allocate some tokens as a start */
	tok = malloc(sizeof(*tok) * tokcount);
	if (tok == NULL) {
		fprintf(stderr, "malloc(): errno=%d\n", errno);
		return 3;
	}

	for (;;) {
		/* Read another chunk */
		r = fread(buf, 1, sizeof(buf), stdin);
		if (r < 0) {
			fprintf(stderr, "fread(): %d, errno=%d\n", r, errno);
			return 1;
		}
		if (r == 0) {
			if (eof_expected != 0) {
				return 0;
			} else {
				fprintf(stderr, "fread(): unexpected EOF\n");
				return 2;
			}
		}

		js = realloc_it(js, jslen + r + 1);
		if (js == NULL) {
			return 3;
		}
		strncpy(js + jslen, buf, r);
		jslen = jslen + r;

again:
		r = jsmn_parse(&p, js, jslen, tok, tokcount);
		if (r < 0) {
			if (r == JSMN_ERROR_NOMEM) {
				tokcount = tokcount * 2;
				tok = realloc_it(tok, sizeof(*tok) * tokcount);
				if (tok == NULL) {
					return 3;
				}
				goto again;
			}
		} else {
			dump(js, tok, 0);
                        printf("\n");
			eof_expected = 1;
		}
	}

	return EXIT_SUCCESS;
}
