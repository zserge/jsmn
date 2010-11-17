/* This demo is not needed to be C89-compatible, so for now GCC extensions are
 * used */
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "jsmn.h"

#define NUM_TOKENS 30

static void jsmn_dump_obj(jsontok_t *obj, const char *js) {
	size_t len;

	if (obj->end < 0 || obj->start < 0) {
		return;
	}

	len = obj->end - obj->start;

	printf("[%3d,%3d]\t", obj->start, obj->end);

	char *type;
	switch (obj->type) {
		case JSON_PRIMITIVE:
			type = "(.)";
			break;
		case JSON_STRING:
			type = "(s)";
			break;
		case JSON_ARRAY:
			type = "(A)";
			break;
		case JSON_OBJECT:
			type = "(O)";
			break;
	}

	printf("%s ", type);

	char *s = strndup((const char *) &js[obj->start], len);
	printf("%s\n", s);
	free(s);
}

int main(int argc, char *argv[]) {
	int i;
	int r;
	jsontok_t tokens[NUM_TOKENS];
	FILE *f;
	int filesize = 0;
	char *js = NULL;

	if (argc != 2) {
		fprintf(stderr, "Usage: ./demo <file.js>\n");
		exit(EXIT_SUCCESS);
	}

	if (strcmp(argv[1], "-") == 0) {
		f = stdin;
	} else {
		f = fopen(argv[1], "r");
		if (f == NULL) {
			fprintf(stderr, "Failed to open file `%s`\n", argv[1]);
			exit(EXIT_FAILURE);
		}
	}

	while (1) {
		char buf[BUFSIZ];
		r = fread(buf, 1, BUFSIZ, f);
		if (r <= 0) {
			break;
		}
		js = (char *) realloc(js, filesize + r);
		if (js == NULL) {
			fprintf(stderr, "Cannot allocate anough memory\n");
			fclose(f);
			exit(EXIT_FAILURE);
		}
		memcpy(js + filesize, buf, r);
		filesize += r;
	}

	fclose(f);

	jsmn_parser parser;

	jsmn_init_parser(&parser, js, tokens, NUM_TOKENS);

	r = jsmn_parse(&parser);
	if (r < 0) {
		printf("error %d at pos %d: %s\n", r, parser.pos, &js[parser.pos]);
	}

	for (i = 0; i<NUM_TOKENS; i++) {
		jsmn_dump_obj(&parser.tokens[i], js);
	}

	free(js);

	return 0;
}
