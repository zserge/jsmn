/* This demo is not needed to be C89-compatible, so for now GCC extensions are
 * used */
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "jsmn.h"

#define NUM_TOKENS 20

static void json_dump_obj(jsontok_t *obj, const unsigned char *js) {
	size_t len;

	printf("[%d,%d]", obj->start, obj->end);
	len = (size_t) (obj->end - obj->start);

	char *type;
	switch (obj->type) {
		case JSON_OTHER:
			type = "other";
			break;
		case JSON_NUMBER:
			type = "number";
			break;
		case JSON_STRING:
			type = "string";
			break;
		case JSON_ARRAY:
			type = "array";
			break;
		case JSON_OBJECT:
			type = "object";
			break;
	}

	printf(" %s ", type);

	if (len > 0) {
			char *s = strndup((const char *) &js[obj->start], len);
			printf("%s", s);
			free(s);
	}
	printf("\n");
}

int main(void) {
	int i;
	jsontok_t tokens[NUM_TOKENS];

	const unsigned char *js = (unsigned char *)
		"{"
			"\"foo\": \"bar\","
			"\"bar\": [1,2, 3],"
			"\"obj\": { \"true\": false}"
		"}";

	jsmn_parse(js, tokens, NUM_TOKENS, NULL);

	for (i = 0; i<NUM_TOKENS; i++) {
		json_dump_obj(&tokens[i], js);
	}

	return 0;
}
