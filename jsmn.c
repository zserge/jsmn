#include <stdlib.h>

#include "jsmn.h"

void jsmn_init_parser(jsmn_parser *parser, const char *js, 
                      jsontok_t *tokens, size_t num_tokens) {
	unsigned int i;

	parser->js = js;
	parser->pos = 0;
	parser->tokens = tokens;
	parser->num_tokens = num_tokens;

	for (i = 0; i < parser->num_tokens; i++) {
		parser->tokens[i].start = -1;
		parser->tokens[i].end = -1;
		parser->tokens[i].type = JSON_PRIMITIVE;
	}
}

jsontok_t *jsmn_start_token(jsmn_parser *parser, jsontype_t type) {
	unsigned int i;
	jsontok_t *tokens = parser->tokens;
	for (i = 0; i<parser->num_tokens; i++) {
		if (tokens[i].start == -1 && tokens[i].end == -1) {
			tokens[i].start = parser->pos;
			tokens[i].type = type;
			return &tokens[i];
		}
	}
	return NULL;
}

jsontok_t *jsmn_end_token(jsmn_parser *parser, jsontype_t type) {
	int i;
	jsontok_t *tokens = parser->tokens;
	for (i = parser->num_tokens - 1; i>= 0; i--) {
		if (tokens[i].type == type && tokens[i].start != -1 && tokens[i].end == -1) {
			tokens[i].end = parser->pos;
			return &tokens[i];
		}
	}
	return NULL;
}

static int jsmn_parse_primitive(jsmn_parser *parser) {
	const char *js;
	jsontok_t *token;

	js = parser->js;

	token = jsmn_start_token(parser, JSON_PRIMITIVE);

	for (; js[parser->pos] != '\0'; parser->pos++) {
		switch (js[parser->pos]) {
			case '\t' : case '\r' : case '\n' : case ' ' :
			case ','  : case ']'  : case '}' :
				token->end = parser->pos;
				parser->pos--;
				return JSMN_SUCCESS;
		}
		if (js[parser->pos] < 32 || js[parser->pos] >= 127) {
			return JSMN_ERROR_INVAL;
		}
	}
	return JSMN_ERROR_PART;
}

static int jsmn_parse_string(jsmn_parser *parser) {
	const char *js;
	jsontok_t *token;

	js = parser->js;

	/* Check if string begins from a quote */
	if (js[parser->pos] != '\"') {
		return JSMN_ERROR_INVAL;
	}

	parser->pos++;

	token = jsmn_start_token(parser, JSON_STRING);
	/* Skip starting quote */
	for (; js[parser->pos] != '\0'; parser->pos++) {
		char c = js[parser->pos];

		/* Quote: end of string */
		if (c == '\"') {
			token->end = parser->pos;
			return JSMN_SUCCESS;
		}

		/* Backslash: Quoted symbol expected */
		if (c == '\\') {
			parser->pos++;
			switch (js[parser->pos]) {
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
					return JSMN_ERROR_INVAL;
			}
		}
	}
	return JSMN_ERROR_PART;
}

jsmnerr_t jsmn_parse(jsmn_parser *parser) {
	const char *js;
	jsontype_t type;
	jsontok_t *token;

	js = parser->js;

	for (; js[parser->pos] != '\0'; parser->pos++) {
		char c;
		c = js[parser->pos];
		switch (c) {
			case '{': case '[':
				type = (c == '{' ? JSON_OBJECT : JSON_ARRAY);
				token = jsmn_start_token(parser, type);
				break;
			case '}': case ']':
				type = (c == '}' ? JSON_OBJECT : JSON_ARRAY);
				token = jsmn_end_token(parser, type);
				token->end++;
				break;
			case '-': case '0': case '1' : case '2': case '3' : case '4':
			case '5': case '6': case '7' : case '8': case '9':
			case 't': case 'f': case 'n' :
				jsmn_parse_primitive(parser);
				break;
			case '\"':
				jsmn_parse_string(parser);
				break;
			case '\t' : case '\r' : case '\n' : case ':' : case ',': case ' ': 
				break;
			default:
				return JSMN_ERROR_INVAL;
		}
	}
	return JSMN_SUCCESS;
}

