#ifndef __JSMN_H_
#define __JSMN_H_

/**
 * JSON type identifier. Basic types are:
 * 	o Object
 * 	o Array
 * 	o String
 * 	o Other primitive: number, boolean (true/false) or null
 */
typedef enum {
	JSMN_PRIMITIVE = 0,
	JSMN_OBJECT = 1,
	JSMN_ARRAY = 2,
	JSMN_STRING = 3
} jsmntype_t;

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
	jsmntype_t type;
	int start;
	int end;
	int size;
} jsmntok_t;

/**
 * JSON parser. Contains an array of token blocks available. Also stores
 * the string being parsed now and current position in that string
 */
typedef struct {
	const char *js;
	unsigned int pos;
	unsigned int num_tokens;
	int curtoken;
	int *cursize;
	jsmntok_t *tokens;
} jsmn_parser;

/**
 * Create JSON parser over an array of tokens
 */
void jsmn_init_parser(jsmn_parser *parser, const char *js, 
                      jsmntok_t *tokens, unsigned int num_tokens);

/**
 * Run JSON parser. It parses a JSON data string into and array of tokens, each describing
 * a single JSON object.
 */
jsmnerr_t jsmn_parse(jsmn_parser *parser);

#endif /* __JSMN_H_ */
