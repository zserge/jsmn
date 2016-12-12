#include "jsmn.h"

#define PARENT tokens[parser->toksuper]

// The first child node is always next sequentially.
#define JSMN_DESCEND { \
    tokens[parser->toknext-1].skip = -parser->toksuper-1; \
    parser->toksuper = parser->toknext-1; \
}
#define JSMN_ASCEND { \
    int last_child = parser->toksuper; \
    parser->toksuper = -tokens[last_child].skip-1; \
    tokens[last_child].skip = parser->toknext - last_child; \
}

/**
 * Allocates a fresh unused token from the token pool.
 */
static jsmntok_t *jsmn_alloc_token(jsmn_parser *parser,
		jsmntok_t *tokens, size_t num_tokens) {
	jsmntok_t *tok;
	if (parser->toknext >= num_tokens) {
		return NULL;
	}
        //tok->type = JSMN_UNKNOWN;
	tok = &tokens[parser->toknext++];
	tok->start = tok->end = -1;
        tok->skip = 1;
#ifdef USE_PARENT_LINKS
        tok->parent = parser->toksuper;
#endif

	return tok;
}

/**
 * Fills token type and boundaries.
 */
static void jsmn_fill_token(jsmntok_t *token, jsmntype_t type,
                            int start, int end) {
	token->type = type;
	token->start = start;
	token->end = end;
}

/**
 * Fills next available token with JSON primitive.
 */
static int jsmn_parse_primitive(jsmn_parser *parser, const char *js,
		size_t len, jsmntok_t *tokens, size_t num_tokens) {
	jsmntok_t *token;
	int start;

	start = parser->pos;

	for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
		switch (js[parser->pos]) {
#ifndef JSMN_STRICT
			/* In strict mode primitive must be followed by "," or "}" or "]" */
			case ':':
#endif
			case '\t' : case '\r' : case '\n' : case ' ' :
			case ','  : case ']'  : case '}' :
				goto found;
		}
		if (js[parser->pos] < 32 || js[parser->pos] >= 127) {
			parser->pos = start;
			return JSMN_ERROR_INVAL;
		}
	}
#ifdef JSMN_STRICT
	/* In strict mode primitive must be followed by a comma/object/array */
	parser->pos = start;
	return JSMN_ERROR_PART;
#endif

found:
	if (tokens == NULL) {
		parser->pos--;
		return 0;
	}
	token = jsmn_alloc_token(parser, tokens, num_tokens);
	if (token == NULL) {
		parser->pos = start;
		return JSMN_ERROR_NOMEM;
	}
	jsmn_fill_token(token, JSMN_PRIMITIVE, start, parser->pos);
	parser->pos--;
	return 0;
}

/**
 * Fills next token with JSON string.
 */
static int jsmn_parse_string(jsmn_parser *parser, const char *js,
		size_t len, jsmntok_t *tokens, size_t num_tokens) {
	jsmntok_t *token;

	int start = parser->pos;

	parser->pos++;

	/* Skip starting quote */
	for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
		char c = js[parser->pos];

		/* Quote: end of string */
		if (c == '\"') {
			if (tokens == NULL) {
				return 0;
			}
			token = jsmn_alloc_token(parser, tokens, num_tokens);
			if (token == NULL) {
				parser->pos = start;
				return JSMN_ERROR_NOMEM;
			}
			jsmn_fill_token(token, JSMN_STRING, start+1, parser->pos);
			return 0;
		}

		/* Backslash: Quoted symbol expected */
		if (c == '\\' && parser->pos + 1 < len) {
			int i;
			parser->pos++;
			switch (js[parser->pos]) {
				/* Allowed escaped symbols */
				case '\"': case '/' : case '\\' : case 'b' :
				case 'f' : case 'r' : case 'n'  : case 't' :
					break;
				/* Allows escaped symbol \uXXXX */
				case 'u':
					parser->pos++;
					for(i = 0; i < 4 && parser->pos < len && js[parser->pos] != '\0'; i++) {
						/* If it isn't a hex character we have an error */
						if(!((js[parser->pos] >= 48 && js[parser->pos] <= 57) || /* 0-9 */
									(js[parser->pos] >= 65 && js[parser->pos] <= 70) || /* A-F */
									(js[parser->pos] >= 97 && js[parser->pos] <= 102))) { /* a-f */
							parser->pos = start;
							return JSMN_ERROR_INVAL;
						}
						parser->pos++;
					}
					parser->pos--;
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

/**
 * Parse JSON string and fill tokens.
 */
int jsmn_parse(jsmn_parser *parser, const char *js, size_t len,
		jsmntok_t *tokens, unsigned int num_tokens) {
	int r;
	int i;
	jsmntok_t *token;
	int count = parser->toknext;

	for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
		char c;
		jsmntype_t type;

		c = js[parser->pos];
		switch (c) {
			case '{': case '[':
				count++;
				if (tokens == NULL) {
					break;
				}
                                // Definitely can't use these as keys
                                // (would not be able to ascend).
				if(parser->toksuper != -1
                                    && PARENT.type == JSMN_OBJECT) {
						return JSMN_ERROR_INVAL;
				}
				token = jsmn_alloc_token(parser, tokens, num_tokens);
				if (token == NULL)
					return JSMN_ERROR_NOMEM;
				token->type = (c == '{' ? JSMN_OBJECT : JSMN_ARRAY);
				token->start = parser->pos;
                                JSMN_DESCEND;
				break;
			case '}': case ']':
				if (tokens == NULL)
					break;
				type = (c == '}' ? JSMN_OBJECT : JSMN_ARRAY);

                                // Ascend one key:val pair relationship (if appl.).
                                if(parser->toksuper != -1 && PARENT.end != -1) {
                                    JSMN_ASCEND;
                                }
                                if(parser->toksuper == -1 || PARENT.type != type)
                                    return JSMN_ERROR_INVAL;

                                PARENT.end = parser->pos + 1;
                                JSMN_ASCEND;
				break;
			case '\"':
				r = jsmn_parse_string(parser, js, len, tokens, num_tokens);
				if (r < 0) return r;
				count++;
				break;
			case '\t' : case '\r' : case '\n' : case ' ':
				break;
                        case ':': // Note: no state => {'a' : 'b', : 'c'} parses
                                  // as {'a' : 'b', 'b' : 'c'}...
                                if (parser->toknext < 1) {
					return JSMN_ERROR_INVAL;
                                }
                                if (tokens == NULL) break;

				if (parser->toksuper == -1
                                    || PARENT.type != JSMN_OBJECT
                                    || parser->toksuper == parser->toknext-1) {
					return JSMN_ERROR_INVAL;
				}
                                // Key is the parent.
                                JSMN_DESCEND;
				break;
			case ',':
				if (parser->toksuper != -1
				    && PARENT.type != JSMN_ARRAY
				    && PARENT.type != JSMN_OBJECT) {
                                    // Parent was key in a key:val pair.
                                        JSMN_ASCEND;
				}
				break;
#ifdef JSMN_STRICT
			/* In strict mode primitives are: numbers and booleans */
			case '-': case '0': case '1' : case '2': case '3' : case '4':
			case '5': case '6': case '7' : case '8': case '9':
			case 't': case 'f': case 'n' :
				/* And they must not be keys of the object */
				if (tokens != NULL
                                    && parser->toksuper != -1
                                    && PARENT.type == JSMN_OBJECT) {
						return JSMN_ERROR_INVAL;
				}
#else
			/* In non-strict mode every unquoted value is a primitive */
			default:
#endif
				r = jsmn_parse_primitive(parser, js, len, tokens, num_tokens);
				if (r < 0) return r;
				count++;
				break;

#ifdef JSMN_STRICT
			/* Unexpected char in strict mode */
			default:
				return JSMN_ERROR_INVAL;
#endif
		}
	}

	if (tokens != NULL) {
            if(parser->toksuper != -1) {
		/* Unmatched opened object or array */
		return JSMN_ERROR_PART;
            }
	}

	return count;
}

/**
 * Creates a new parser based over a given  buffer with an array of tokens
 * available.
 */
void jsmn_init(jsmn_parser *parser) {
    static jsmntok_t *garbage;

    parser->pos = 0;
    parser->toknext = 0;
    parser->toksuper = -1;
}

