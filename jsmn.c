#include <stdlib.h>
#include <stdio.h>

#include "jsmn.h"

enum {
	JSON_SKIP = 0,
	JSON_OPEN = 1,
	JSON_CLOSE = 2,
	JSON_BARE = 3,
	JSON_UNBARE = 4,
	JSON_QUOTE = 5,
	JSON_UNQUOTE = 6,
	JSON_ERROR = 7
};

#define JSON_SYM_SKIP(sym)	\
	[sym] = JSON_SKIP

#define JSON_SYM_ERROR(sym)	\
	[sym] = JSON_ERROR

#define JSON_SYM_OPEN(sym)	\
	[sym] = JSON_OPEN

#define JSON_SYM_CLOSE(sym)	\
	[sym] = JSON_CLOSE

#define JSON_SYM_BARE(sym)	\
	[sym] = JSON_BARE

#define JSON_SYM_UNBARE(sym)	\
	[sym] = JSON_UNBARE

#define JSON_SYM_QUOTE(sym)	\
	[sym] = JSON_QUOTE

#define JSON_SYM_UNQUOTE(sym)	\
	[sym] = JSON_UNQUOTE

int jsmn_parse(const unsigned char *js, jsontok_t *tokens, size_t num_tokens, int **errpos) {

	int jsmn_token_start(jsontype_t type, int pos) {
		unsigned int i;
		for (i = 0; i<num_tokens; i++) {
			if (tokens[i].start == -1 && tokens[i].end == -1) {
				tokens[i].start = pos;
				tokens[i].type = type;
				return 0;
			}
		}
		return -1;
	}

	int jsmn_token_end(jsontype_t type, int pos) {
		unsigned int i;
		for (i = num_tokens - 1; i>= 0; i--) {
			if (tokens[i].type == type && tokens[i].start != -1 && tokens[i].end == -1) {
				tokens[i].end = pos;
				return 0;
			}
		}
		return -1;
	}
	
	const unsigned char *p;
	jsontok_t *cur_token;

	int obj_common[] = {
		JSON_SYM_ERROR(0 ... 255),
		JSON_SYM_SKIP('\t'), JSON_SYM_SKIP('\r'),JSON_SYM_SKIP('\n'),
		JSON_SYM_SKIP(':'), JSON_SYM_SKIP(','), JSON_SYM_SKIP(' '),
		JSON_SYM_QUOTE('\"'),
		JSON_SYM_OPEN('['), JSON_SYM_CLOSE(']'),
		JSON_SYM_OPEN('{'), JSON_SYM_CLOSE('}'),
		JSON_SYM_BARE('-'), JSON_SYM_BARE('0'...'9'),
		JSON_SYM_BARE('t'), JSON_SYM_BARE('f'), JSON_SYM_BARE('n') /* true false null */
	};

	int obj_bare[] = {
		JSON_SYM_ERROR(0 ... 31),
		JSON_SYM_ERROR(127 ... 255),
		JSON_SYM_SKIP(32 ... 126),
		JSON_SYM_UNBARE('\t'), JSON_SYM_UNBARE(' '),
		JSON_SYM_UNBARE('\r'), JSON_SYM_UNBARE('\n'),
		JSON_SYM_UNBARE(','), JSON_SYM_UNBARE(']'),
		JSON_SYM_UNBARE('}')
	};

	int obj_string[] = {
		JSON_SYM_ERROR(0 ... 31), JSON_SYM_ERROR(127),
		JSON_SYM_SKIP(32 ... 126),
		JSON_SYM_UNQUOTE('\"'),
		JSON_SYM_ERROR(248 ... 255),
	};

	int *obj_state = obj_common;

	cur_token = tokens;

	int i;
	for (i = 0; i<num_tokens; i++) {
		tokens[i].start = tokens[i].end = -1;
		tokens[i].type = JSON_OTHER;
	}

	for (p = js; *p != '\0'; ) {
		switch (obj_state[*p]) {
			case JSON_ERROR:
				if (errpos != NULL) {
					*errpos = p;
				}
				return -1;

			case JSON_OPEN:
				jsmn_token_start(JSON_OBJECT, p - js);
				break;
			case JSON_CLOSE:
				jsmn_token_end(JSON_OBJECT, p - js + 1);
				break;

			case JSON_BARE:
				jsmn_token_start(JSON_OTHER, p - js);
				obj_state = obj_bare;
				break;
			case JSON_UNBARE:
				jsmn_token_end(JSON_OTHER, p - js);
				obj_state = obj_common;
				continue;

			case JSON_QUOTE:
				jsmn_token_start(JSON_STRING, p - js + 1);
				obj_state = obj_string;
				break;
			case JSON_UNQUOTE:
				jsmn_token_end(JSON_STRING, p - js);
				obj_state = obj_common;
				break;
			case JSON_SKIP:
				break;
		}
		p++;
	}
	return 0;
}

void json_dump_obj(jsontok_t *obj, const char *js) {
	int len;

	printf("[%d,%d]", obj->start, obj->end);
	len = obj->end - obj->start;

	char *type;
	switch (obj->type) {
		case JSON_OTHER:
			type = "other";
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
			char *s = strndup(&js[obj->start], len);
			printf("%s", s);
			free(s);
	}
	printf("\n");
}

int main(int argc, char *argv[]) {
	int i;
#define NUM_TOKENS 20
	jsontok_t tokens[NUM_TOKENS];

	const char *js = 
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
