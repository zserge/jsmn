#include <stdlib.h>

#include "jsmn.h"

static jsontok_t *jsmn_get_token(jsmn_parser *parser) {
	unsigned int i;
	jsontok_t *tokens = parser->tokens;
	for (i = parser->curtoken; i<parser->num_tokens; i++) {
		if (tokens[i].start == -1 && tokens[i].end == -1) {
			parser->curtoken = i;
			return &tokens[i];
		}
	}
	return NULL;
}

static void jsmn_fill_token(jsontok_t *token, jsontype_t type, int start, int end) {
	token->type = type;
	token->start = start;
	token->end = end;
}

void jsmn_init_parser(jsmn_parser *parser, const char *js, 
                      jsontok_t *tokens, unsigned int num_tokens) {
	unsigned int i;

	parser->js = js;
	parser->pos = 0;
	parser->tokens = tokens;
	parser->num_tokens = num_tokens;
	parser->curtoken = 0;

	for (i = 0; i < parser->num_tokens; i++) {
		jsmn_fill_token(&parser->tokens[i], JSON_PRIMITIVE, -1, -1);
	}
}

static int jsmn_parse_primitive(jsmn_parser *parser) {
	const char *js;
	jsontok_t *token;
	int start;

	start = parser->pos;

	js = parser->js;

	for (; js[parser->pos] != '\0'; parser->pos++) {
		switch (js[parser->pos]) {
			case '\t' : case '\r' : case '\n' : case ' ' :
			case ','  : case ']'  : case '}' :
				token = jsmn_get_token(parser);
				if (token == NULL)
					return JSMN_ERROR_NOMEM;
				jsmn_fill_token(token, JSON_PRIMITIVE, start, parser->pos);
				parser->pos--;
				return JSMN_SUCCESS;
		}
		if (js[parser->pos] < 32 || js[parser->pos] >= 127) {
			parser->pos = start;
			return JSMN_ERROR_INVAL;
		}
	}
	parser->pos = start;
	return JSMN_ERROR_PART;
}

static int jsmn_parse_string(jsmn_parser *parser) {
	const char *js;
	jsontok_t *token;

	int start = parser->pos;

	js = parser->js;

	parser->pos++;

	/* Skip starting quote */
	for (; js[parser->pos] != '\0'; parser->pos++) {
		char c = js[parser->pos];

		/* Quote: end of string */
		if (c == '\"') {
			token = jsmn_get_token(parser);
			if (token == NULL)
				return JSMN_ERROR_NOMEM;
			jsmn_fill_token(token, JSON_PRIMITIVE, start+1, parser->pos);
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
					parser->pos = start;
					return JSMN_ERROR_INVAL;
			}
		}
	}
	parser->pos = start;
	return JSMN_ERROR_PART;
}

jsmnerr_t jsmn_parse(jsmn_parser *parser) {
	int r;
	unsigned int i;
	const char *js;
	jsontype_t type;
	jsontok_t *token;

	js = parser->js;

	for (; js[parser->pos] != '\0'; parser->pos++) {
		char c;
		c = js[parser->pos];
		switch (c) {
			case '{': case '[':
				token = jsmn_get_token(parser);
				if (token == NULL)
					return JSMN_ERROR_NOMEM;
				token->type = (c == '{' ? JSON_OBJECT : JSON_ARRAY);
				token->start = parser->pos;
				break;
			case '}': case ']':
				type = (c == '}' ? JSON_OBJECT : JSON_ARRAY);
				for (i = parser->curtoken; i >= 0; i--) {
					token = &parser->tokens[i];
					if (token->start != -1 && token->end == -1) {
						if (token->type != type) {
							return JSMN_ERROR_INVAL;
						}
						token->end = parser->pos + 1;
						break;
					}
				}
				break;
			case '-': case '0': case '1' : case '2': case '3' : case '4':
			case '5': case '6': case '7' : case '8': case '9':
			case 't': case 'f': case 'n' :
				r = jsmn_parse_primitive(parser);
				if (r < 0) return r;
				break;
			case '\"':
				r = jsmn_parse_string(parser);
				if (r < 0) return r;
				break;
			case '\t' : case '\r' : case '\n' : case ':' : case ',': case ' ': 
				break;
			default:
				return JSMN_ERROR_INVAL;
		}
	}
	return JSMN_SUCCESS;
}

