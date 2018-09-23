#include "jsmn.h"

static jsmnint_t _jsmn_alloc_token(jsmn_parser *parser, jsmntok_t *tokens, jsmnint_t num_tokens){
	if (parser->toknext >= num_tokens)
		return JSMN_NULL;

	jsmnint_t tok = parser->toknext++;

	tokens[tok].start = JSMN_NULL;
	tokens[tok].end = JSMN_NULL;
	tokens[tok].parent = JSMN_NULL;

	return tok;
}

static int _jsmn_match(jsmn_parser *parser, const char *js, jsmnint_t len, jsmntok_t *token, const char *m){
	int i;
	for (i = parser->pos; i < len; i++) {
		char c = js[i];
		int k = i-parser->pos;

		if (m[k] == '\0') {
			if (c == ','||c == ']'||c == '}'||c == ' '||c == '\t'||c == '\n') {
				token->end = i;
				parser->pos = i-1;
				return 0;
			}

			return JSMN_ERROR_INVAL;
		}

		if (m[k] != c)
			return JSMN_ERROR_INVAL;
	}

	return JSMN_ERROR_PART;
}

static int _jsmn_parse_primitive(jsmn_parser *parser, const char *js, jsmnint_t len, jsmntok_t *tokens, jsmnint_t num_tokens){
	char c;
	jsmnint_t tk;

	tk = _jsmn_alloc_token(parser, tokens, num_tokens);
	if (tk == JSMN_NULL) {
		return JSMN_ERROR_NOMEM;
	}

	tokens[tk].start = parser->pos;
	tokens[tk].type = JSMN_PRIMITIVE;
	tokens[tk].parent = parser->toksuper;

	c = _jsmn_match(parser, js, len, &tokens[tk], "true");
	if (c == 0)
		return 0;
	else if (c == JSMN_ERROR_PART)
		return JSMN_ERROR_PART;

	c = _jsmn_match(parser, js, len, &tokens[tk], "false");
	if (c == 0)
		return 0;
	else if (c == JSMN_ERROR_PART)
		return JSMN_ERROR_PART;

	c = _jsmn_match(parser, js, len, &tokens[tk], "null");
	if (c == 0)
		return 0;
	else if (c == JSMN_ERROR_PART)
		return JSMN_ERROR_PART;

	if (js[parser->pos] == '-')
		parser->pos++;

	for (; parser->pos < len; parser->pos++) {
		c = js[parser->pos];

		if (c == '\0')
			return JSMN_ERROR_INVAL;

		if ((c >= '0' && c <= '9') || c=='e' || c=='E' || c=='.') {
			//ok
		} else if (c == ','||c == ']'||c == '}'||c == ' '||c == '\t'||c == '\n') {

			tokens[tk].end = parser->pos;

			parser->pos--;
			return 0;
		} else {
			return JSMN_ERROR_INVAL;
		}
	}

	return JSMN_ERROR_PART;
}

static int _jsmn_parse_string(jsmn_parser *parser, const char *js, jsmnint_t len, jsmntok_t *tokens, jsmnint_t num_tokens){
	int i;
	int k;
	char c;
	jsmnint_t tk;

	if (parser->key&0x80) {
		tk = parser->toknext-1;
	} else {
		tk = _jsmn_alloc_token(parser, tokens, num_tokens);
		if (tk == JSMN_NULL)
			return JSMN_ERROR_NOMEM;

		parser->key |= 0x80;
		parser->pos++;

		tokens[tk].type = JSMN_STRING;
		tokens[tk].start = parser->pos;
		tokens[tk].parent = parser->toksuper;
	}

	for (; parser->pos < len; parser->pos++) {
		c = js[parser->pos];

		if (c == '\0')
			return JSMN_ERROR_INVAL;

		if (c & 0x80)
			goto GET_UNICODE;

		/* Quote: end of string */
		if (c == '\"') {
			tokens[tk].end = parser->pos;
			parser->key &= ~0x80;
			return 0;
		}

		/* Backslash: Quoted symbol expected */
		if (c == '\\') {
			if (parser->pos >= len - 1)
				return JSMN_ERROR_PART;

			c = js[parser->pos + 1];

			if (c > 127) {
				parser->pos++;
				goto GET_UNICODE;

			} else if (c == 127) //DEL control char
				return JSMN_ERROR_INVAL;

			if (c < 32)	//any control char (except DEL)
				return JSMN_ERROR_INVAL;

			switch (c) {
			case 'u':
				//get 4 chars!
				//pos is the bslash
				//pos+1 is the u
				for (i = 2; i < 6 && parser->pos + i < len; i++) {
					c = js[parser->pos + i];
				    if (c >= '0' && c <= '9')
						continue;
					if (c >= 'A' && c <= 'F')
						continue;
					if (c >= 'a' && c <= 'f')
						continue;
					return JSMN_ERROR_INVAL;
				}
				if (i < 6)
					return JSMN_ERROR_PART;

				parser->pos += 5;
				break;
			case '"':
			case 'b':
			case 'f':
			case 'n':
			case 'r':
			case 't':
			case '/':
			case '\\':
				parser->pos++;
				break;
			default:
				return JSMN_ERROR_INVAL;
			}
		}
		continue;
GET_UNICODE:
		//c is first unicode char
		if ((c & 0xE0) == 0xC0) {			//two
			k=1;
		} else if ((c & 0xF0) == 0xE0) {	//three
			k=2;
		} else if ((c & 0xF8) == 0xF0) {	//four
			k=3;
		} else {
			return JSMN_ERROR_INVAL;
		}

		for (i = 0;i < k && parser->pos + i < len; i++) {
			c = js[parser->pos + i];

			if ((c & 0xC0) != 0x80)
				return JSMN_ERROR_INVAL;
		}

		if (i != 0)
			return JSMN_ERROR_PART;

		parser->pos += k;
	}

	return JSMN_ERROR_PART;
}

#define _JSMN_EXPECT_VALUE  0
#define _JSMN_EXPECT_KEY    1
#define _JSMN_EXPECT_COLON  2
#define _JSMN_EXPECT_COMMA  3

int jsmn_parse(jsmn_parser *parser, const char *js, jsmnint_t len, jsmntok_t *tokens, jsmnint_t num_tokens){
	int r;
	char c;
	jsmnint_t tk;

	if (parser->key & 0x80)
		goto CONTINUE_STRING;

	for (; parser->pos < len; parser->pos++) {
		c = js[parser->pos];

		switch(c) {
		case '\0': return JSMN_ERROR_INVAL;
		case '{':
			if (parser->key != _JSMN_EXPECT_VALUE)
				return JSMN_ERROR_INVAL;

			tk = _jsmn_alloc_token(parser, tokens, num_tokens);
			if (tk == JSMN_NULL)
				return JSMN_ERROR_NOMEM;

			tokens[tk].start = parser->pos;
			tokens[tk].type = JSMN_OBJECT;
			tokens[tk].parent = parser->toksuper;

			parser->toksuper = parser->toknext - 1;
			parser->key = _JSMN_EXPECT_KEY;
			break;
		case '}':
			if (parser->toksuper == JSMN_NULL)
				return JSMN_ERROR_INVAL;

			if (tokens[parser->toksuper].type != JSMN_OBJECT)
				return JSMN_ERROR_INVAL;

			//if (parser->key == _JSMN_EXPECT_KEY && parent->size>0)	//trailing ,
			//	return JSMN_ERROR_INVAL;

			tokens[parser->toksuper].end = parser->pos + 1;
			parser->toksuper = tokens[parser->toksuper].parent;

			if (parser->toksuper == JSMN_NULL)
				return parser->toknext;

			parser->key = _JSMN_EXPECT_COMMA;
			break;
		case '[':
			if (parser->key != _JSMN_EXPECT_VALUE)
				return JSMN_ERROR_INVAL;

			tk = _jsmn_alloc_token(parser, tokens, num_tokens);
			if (tk == JSMN_NULL)
				return JSMN_ERROR_NOMEM;

			tokens[tk].start = parser->pos;
			tokens[tk].type = JSMN_ARRAY;
			tokens[tk].parent = parser->toksuper;

			parser->toksuper = parser->toknext - 1;
			parser->key = _JSMN_EXPECT_VALUE;
			break;
		case ']':
			if (parser->toksuper == JSMN_NULL)
				return JSMN_ERROR_INVAL;

			if (tokens[parser->toksuper].type != JSMN_ARRAY)
				return JSMN_ERROR_INVAL;

			//if (parser->key == _JSMN_EXPECT_KEY && parent->size>0)	//trailing ,
			//	return JSMN_ERROR_INVAL;

			tokens[parser->toksuper].end = parser->pos + 1;
			parser->toksuper = tokens[parser->toksuper].parent;

			if (parser->toksuper == JSMN_NULL)
				return parser->toknext;

			parser->key = _JSMN_EXPECT_COMMA;
			break;
		case '"':
			if (parser->key != _JSMN_EXPECT_VALUE && parser->key != _JSMN_EXPECT_KEY)
				return JSMN_ERROR_INVAL;
CONTINUE_STRING:

			r = _jsmn_parse_string(parser, js, len, tokens, num_tokens);
			if (r != 0)
				return r;

			if (parser->key == _JSMN_EXPECT_KEY)
				parser->key = _JSMN_EXPECT_COLON;
			else if (parser->key == _JSMN_EXPECT_VALUE)
				parser->key = _JSMN_EXPECT_COMMA;
			break;
		case ',':
			if (parser->key != _JSMN_EXPECT_COMMA)
				return JSMN_ERROR_INVAL;

			if (parser->toksuper == JSMN_NULL)
				return JSMN_ERROR_INVAL;

			if (tokens[parser->toksuper].type == JSMN_OBJECT)
				parser->key = _JSMN_EXPECT_KEY;
			else
				parser->key = _JSMN_EXPECT_VALUE;

			break;
		case ':':
			if (parser->key != _JSMN_EXPECT_COLON)
				return JSMN_ERROR_INVAL;

			parser->key = _JSMN_EXPECT_VALUE;

			break;
		case '\t':
		case '\n':
		case ' ':
			break;
		default:
			if (parser->key != _JSMN_EXPECT_VALUE)
				return JSMN_ERROR_INVAL;

			r = _jsmn_parse_primitive(parser, js, len, tokens, num_tokens);
			if (r != 0)
				return r;

			parser->key = _JSMN_EXPECT_COMMA;

			break;
		}
	}

	return JSMN_ERROR_PART;
}

void jsmn_init(jsmn_parser *parser){
	parser->pos = 0;
	parser->toknext = 0;
	parser->key = _JSMN_EXPECT_VALUE;
	parser->toksuper = JSMN_NULL;
}

