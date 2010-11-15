#include <stdlib.h>

#include "jsmn.h"

struct jsmn_params {
	jsontok_t *tokens;
	size_t num_tokens;
	int *errpos;
};

/**
 * Read the string from JSON data. Store string 
 */
static int jsmn_parse_string(const unsigned char *js, jsontok_t *token) {
	const unsigned char *p;

	/* Check if string begins from a quote */
	if (js[token->start] != '\"') {
		return -1;
	}

	/* Skip starting quote */
	token->start++;

	for (p = &js[token->start]; *p != '\0'; p++) {
		/* Quote: end of string */
		if (*p == '\"') {
			token->end = p - js;
			return 0;
		}

		/* Backslash: Quoted symbol expected */
		if (*p == '\\') {
			p++;
			switch (*p) {
				/* Allowed escaped symbols */
				case '\"': case '/' : case '\\' : case 'b' :
				case 'f' : case 'r' : case 'n'  : case 't' :
					break;
				/* Allows escaped symbol \uXXXX */
				case 'u':
					/* TODO */
					break;
				/* Unexpected symbol */
				default:
					return -1;
			}
		}
	}
	return -1;
}

static int jsmn_parse_primitive(const unsigned char *js, jsontok_t *token) {
	const unsigned char *p;

	for (p = &js[token->start]; *p != '\0'; p++) {
		switch (*p) {
			case '\t' : case '\r' : case '\n' : case ' ' :
			case ','  : case ']'  : case '}' :
				token->end = p - js;
				return 0;
		}
		if (*p < 32 || *p >= 127) {
			return -1;
		}
	}
	return -1;
}

static void jsmn_error(struct jsmn_params *params, int pos) {
	if (params->errpos != NULL) {
		*params->errpos = pos;
	}
}

static jsontok_t *jsmn_token_start(struct jsmn_params *params, jsontype_t type, int pos) {
	unsigned int i;
	jsontok_t *tokens = params->tokens;
	for (i = 0; i<params->num_tokens; i++) {
		if (tokens[i].start == -1 && tokens[i].end == -1) {
			tokens[i].start = pos;
			tokens[i].type = type;
			return &tokens[i];
		}
	}
	return NULL;
}

static jsontok_t *jsmn_token_end(struct jsmn_params *params, jsontype_t type, int pos) {
	int i;
	jsontok_t *tokens = params->tokens;
	for (i = params->num_tokens - 1; i>= 0; i--) {
		if (tokens[i].type == type && tokens[i].start != -1 && tokens[i].end == -1) {
			tokens[i].end = pos;
			return &tokens[i];
		}
	}
	return NULL;
}

int jsmn_parse(const unsigned char *js, jsontok_t *tokens, size_t num_tokens, int *errpos) {

	struct jsmn_params params;

	int r;
	unsigned int i;
	const unsigned char *p;
	jsontype_t type;
	jsontok_t *cur_token;

	params.num_tokens = num_tokens;
	params.tokens = tokens;
	params.errpos = errpos;

	for (i = 0; i<num_tokens; i++) {
		tokens[i].start = tokens[i].end = -1;
		tokens[i].type = JSON_OTHER;
	}

	for (p = js; *p != '\0'; ) {
		switch (*p) {
			case '{': case '[':
				type = (*p == '{' ? JSON_OBJECT : JSON_ARRAY);
				cur_token = jsmn_token_start(&params, type, p - js);
				cur_token->start = p - js;
				break;
			case '}' : case ']':
				type = (*p == '}' ? JSON_OBJECT : JSON_ARRAY);
				cur_token = jsmn_token_end(&params, type, p - js + 1);
				cur_token->end = p - js + 1;
				break;

			case '-': case '0': case '1' : case '2': case '3' : case '4':
			case '5': case '6': case '7' : case '8': case '9':
			case 't': case 'f': case 'n' :
				cur_token = jsmn_token_start(&params, JSON_OTHER, p - js);
				r = jsmn_parse_primitive(js, cur_token);
				if (r < 0) {
					jsmn_error(&params, p - js);
					return -1;
				}
				p = &js[cur_token->end];
				break;

			case '\"':
				cur_token = jsmn_token_start(&params, JSON_STRING, p - js);
				r = jsmn_parse_string(js, cur_token);
				if (r < 0) {
					jsmn_error(&params, p - js);
					return -1;
				}
				p = &js[cur_token->end];
				break;

			case '\t' : case '\r' : case '\n' : case ':' : case ',': case ' ': 
				break;

			default:
				jsmn_error(&params, p - js);
				return -1;
		}
		p++;
	}
	jsmn_error(&params, 0);
	return 0;
}

