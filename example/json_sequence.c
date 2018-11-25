#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../jsmn.h"

static const char *JSON_STRING =
	"{\"@timestamp\":\"2018-11-25T18:45:00\", \"programname\":\"my_prog\", \"procid\":\"123\", \"severity\":\"info\", \"message\":\"Started\"}\n"
	"{\"@timestamp\":\"2018-11-25T18:45:01\", \"programname\":\"my_prog\", \"procid\":\"123\", \"severity\":\"warn\", \"message\":\"File is too large\"}\n"
	"{\"@timestamp\":\"2018-11-25T18:45:03\", \"programname\":\"oom_killer\", \"procid\":\"42\", \"severity\":\"info\", \"message\":\"Process 123 (my_prog) was killed\"}";

int main() {
	int r;
	jsmn_parser p;
	jsmntok_t t[11]; /* We expect no more than 11 tokens in one JSON object */

	jsmn_init(&p);
	size_t len = strlen(JSON_STRING);

	r = jsmn_parse_next(&p, JSON_STRING, len, t, 11);
	while (r > 0) {
		printf("%.*s %.*s[%.*s]: <%.*s> %.*s\n",
			t[2].end - t[2].start, JSON_STRING + t[2].start,
			t[4].end - t[4].start, JSON_STRING + t[4].start,
			t[6].end - t[6].start, JSON_STRING + t[6].start,
			t[8].end - t[8].start, JSON_STRING + t[8].start,
			t[10].end - t[10].start, JSON_STRING + t[10].start);
		r = jsmn_parse_next(&p, JSON_STRING, len, t, 11);
	}
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
