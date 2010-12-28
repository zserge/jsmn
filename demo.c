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

static void jsmn_dump_obj(jsmntok_t *obj, const char *js) {
	size_t len;
	char *s;

	if (obj->end < 0 || obj->start < 0) {
		return;
	}

	len = obj->end - obj->start;

	printf("[%3d,%3d - %2d] (%c) ", obj->start, obj->end, obj->size,
		({
			char c;
			switch (obj->type) {
				case JSMN_PRIMITIVE: c = '.'; break;
				case JSMN_STRING: c = 's'; break;
				case JSMN_ARRAY: c = 'A'; break;
				case JSMN_OBJECT: c = 'O'; break;
				default: c = '?';
			}; c;
		}));

	s = strndup((const char *) &js[obj->start], len);
	char *p;
	for (p = s; *p; p++) {
		printf("%c", *p == '\n' ? ' ' : *p);
	}
	printf("\n");
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
	jsmntok_t *tokens;
	int block_size = 1024;
	int num_tokens = 100;

	while ((c = getopt(argc, argv, "ht:b:")) != -1) {
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
			case 'b':
				block_size = atoi(optarg);
				if (errno || block_size < 0) {
					fprintf(stderr, "Invalid block size: %s!\n", optarg);
					exit(EXIT_FAILURE);
				}
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

	tokens = malloc(num_tokens * sizeof(jsmntok_t));
	if (tokens == NULL) {
		fprintf(stderr, "Cannot allocate anough memory\n");
		exit(EXIT_FAILURE);
	}

	jsmn_init_parser(&parser, js, tokens, num_tokens);

	char *buf = malloc(block_size);
	while (1) {
		r = fread(buf, 1, block_size, f);
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
	free(buf);
	free(tokens);
	free(js);

	return 0;
}
