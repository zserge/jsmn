#ifndef __JSMN_H_
#define __JSMN_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * JSON type identifier. Basic types are:
 * 	o Object
 * 	o Array
 * 	o String
 * 	o Other primitive: number, boolean (true/false) or null
 */
typedef enum {
	JSMN_UNDEFINED = 0,
	JSMN_OBJECT = 1,
	JSMN_ARRAY = 2,
	JSMN_STRING = 3,
	JSMN_PRIMITIVE = 4
} jsmntype_t;

enum jsmnerr {
	/* Not enough tokens were provided */
	JSMN_ERROR_NOMEM = -1,
	/* Invalid character inside JSON string */
	JSMN_ERROR_INVAL = -2,
	/* The string is not a full JSON packet, more bytes expected */
	JSMN_ERROR_PART = -3
};

typedef unsigned int jsmnint_t;
#define JSMN_NULL ((jsmnint_t)-1)

/**
 * JSON token description.
 * type		type (object, array, string etc.)
 * start	start position in JSON data string
 * end		end position in JSON data string
 * parent	index pr parent token
 */
typedef struct {
	jsmntype_t type;
	jsmnint_t start;
	jsmnint_t end;
	jsmnint_t parent;
} jsmntok_t;

/**
 * JSON parser.
 * pos		offset in the JSON string
 * toknext	next token to allocate
 * toksuper	parent of item currently parsing eg: of an object or array
 * flags	bitflags to aid in parseing
 */
typedef struct {
	jsmnint_t pos;
	jsmnint_t toknext;
	jsmnint_t toksuper;
	char key;
} jsmn_parser;

/**
 * Initialize JSON parser
 */
void jsmn_init(jsmn_parser *parser);

/**
 * Run JSON parser. It parses a JSON data string into and array of tokens,
 * each describing a single JSON object.
 */
int jsmn_parse(jsmn_parser *parser, const char *js, jsmnint_t len,
		jsmntok_t *tokens, jsmnint_t num_tokens);

#ifdef __cplusplus
}
#endif

#endif /* __JSMN_H_ */
