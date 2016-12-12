#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../jsmn.h"

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */

static const char *JSON_STRING =
	"{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000,\n  "
	"\"groups\": [\"users\", \"wheel\", \"audio\", \"video\"]}";

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

int main() {
	int r;
	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */
        jsmntok_t *key;

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}

	/* Assume the top-level element is an object */
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		printf("Object expected\n");
		return 1;
	}

	/* Loop over all keys of the root object */
	for (key=t+1; key < t+r; key += key->skip) {
                jsmntok_t *val = key+1;
                if (val == NULL) {
                        printf("Missing value for %.*s\n", FMT_STR(JSON_STRING, key));
                }
                if(val->type == JSMN_STRING || val->type == JSMN_PRIMITIVE) {
			printf("- %.*s: %.*s\n", FMT_STR(JSON_STRING, key), FMT_STR(JSON_STRING, val));
		} else if (val->type == JSMN_ARRAY) {
                        jsmntok_t *g;
			printf("- %.*s:\n", FMT_STR(JSON_STRING, key));
                        for(g = val+1; g < val+val->skip; g += g->skip) {
                            if(g->type != JSMN_STRING) {
                                    printf("  * <non-string?>\n");
                            } else {
                                    printf("  * %.*s\n", FMT_STR(JSON_STRING, g));
                            }
			}
		} else {
			printf("Unexpected key: %.*s\n", FMT_STR(JSON_STRING, key));
		}
	}
	return EXIT_SUCCESS;
}
