/* This demo is not needed to be C89-compatible, so for now GCC extensions are
 * used */
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>

#include "jsmn.h"

static void jsmn_dump_obj(jsontok_t *obj, const char *js) {
	size_t len;
	char *s;

	if (obj->end < 0 || obj->start < 0) {
		return;
	}

	len = obj->end - obj->start;

	printf("[%3d,%3d] (%c)\t", obj->start, obj->end,
		({
			char c;
			switch (obj->type) {
				case JSON_PRIMITIVE: c = '.'; break;
				case JSON_STRING: c = 's'; break;
				case JSON_ARRAY: c = 'A'; break;
				case JSON_OBJECT: c = 'O'; break;
				default: c = '?';
			}; c;
		}));

	s = strndup((const char *) &js[obj->start], len);
	printf("%s\n", s);
	free(s);
}

void usage(void) {
		fprintf(stderr, "Usage: ./demo <file.js>\n");
		exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
	int i;
	int r;
	int c;

	FILE *f;
	int filesize = 0;

	jsmn_parser parser;
	char *js = NULL;
	jsontok_t *tokens;
	int num_tokens = 100;

	while ((c = getopt(argc, argv, "ht:")) != -1) {
		switch (c) {
			case 'h':
				usage();
				break;
			case 't':
				num_tokens = atoi(optarg);
				if (errno || num_tokens < 0) {
					fprintf(stderr, "Invalid token number: %s!\n", optarg);
					exit(EXIT_FAILURE);
				}
				break;
		}
	}

	if (optind >= argc) {
		usage();
	}

	if (strcmp(argv[optind], "-") == 0) {
		f = stdin;
	} else {
		f = fopen(argv[optind], "r");
		if (f == NULL) {
			fprintf(stderr, "Failed to open file `%s`\n", argv[1]);
			exit(EXIT_FAILURE);
		}
	}

	tokens = malloc(num_tokens * sizeof(jsontok_t));
	if (tokens == NULL) {
		fprintf(stderr, "Cannot allocate anough memory\n");
		exit(EXIT_FAILURE);
	}

	jsmn_init_parser(&parser, js, tokens, num_tokens);

	while (1) {
		char buf[BUFSIZ];
		r = fread(buf, 1, 1, f);
		if (r <= 0) {
			break;
		}
		js = (char *) realloc(js, filesize + r + 1);
		if (js == NULL) {
			fprintf(stderr, "Cannot allocate anough memory\n");
			fclose(f);
			exit(EXIT_FAILURE);
		}
		parser.js = js;

		memcpy(js + filesize, buf, r);
		filesize += r;
		js[filesize] = '\0';

		r = jsmn_parse(&parser);
		if (r < 0) {
			printf("error %d at pos %d: %s\n", r, parser.pos, &js[parser.pos]);
		}

		for (i = 0; i<num_tokens; i++) {
			jsmn_dump_obj(&parser.tokens[i], js);
		}
	}

	fclose(f);
	free(tokens);
	free(js);

	return 0;
}
