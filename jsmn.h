#ifndef __JSMN_H_
#define __JSMN_H_

/**
 * JSON type identifier. Basic types are:
 * 	o Object
 * 	o Array
 * 	o String
 * 	o Number
 * 	o Other primitive: boolean (true/false) or null
 */
typedef enum {
	JSON_PRIMITIVE = 0,
	JSON_OBJECT = 1,
	JSON_ARRAY = 2,
	JSON_STRING = 3
} jsontype_t;

typedef enum {
	JSMN_ERROR_NOMEM = -1,
	JSMN_ERROR_INVAL = -2,
	JSMN_ERROR_PART = -3,
	JSMN_SUCCESS = 0
} jsmnerr_t;

/**
 * JSON token description.
 * @param		type	type (object, array, string etc.)
 * @param		start	start position in JSON data string
 * @param		end		end position in JSON data string
 */
typedef struct {
	jsontype_t type;
	int start;
	int end;
} jsontok_t;

/**
 * JSON parser. Contains an array of token blocks available. Also stores
 * the string being parsed now and current position in that string
 */
typedef struct {
	const char *js;
	unsigned int pos;
	size_t num_tokens;
	jsontok_t *tokens;
} jsmn_parser;

/**
 * Create JSON parser over an array of tokens
 */
void jsmn_init_parser(jsmn_parser *parser, const char *js, 
                      jsontok_t *tokens, size_t num_tokens);

/**
 * Run JSON parser. It parses a JSON data string into and array of tokens, each describing
 * a single JSON object.
 */
jsmnerr_t jsmn_parse(jsmn_parser *parser);

#endif /* __JSMN_H_ */
