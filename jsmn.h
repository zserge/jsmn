#ifndef __JSMN_H_
#define __JSMN_H_

// During parsing, the current token's 'skip' field is used to store
// the location of the parent.
// After parsing, it holds the total number of tokens
// in the node's sub-tree (including itself).
//
// Listing siblings is then as simple as (next = t + t->skip).
//
// { or [ -> push node and DESCEND
// :      -> check lhs was a key and DESCEND
// }(parent={) or ](parent=[) or ,(parent!=[or{) -> ASCEND

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* explicitly */
#define JSMN_CHILD(t) (t->skip == 1 ? NULL : t+1)
#define JSMN_NEXT(t)  (t+t->skip)

#define FMT_STR(str, v) (v)->end-(v)->start, str+(v)->start

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

/**
 * JSON token description.
 * type		type (object, array, string etc.)
 * start	start position in JSON data string
 * end		end position in JSON data string
 */
typedef struct {
	jsmntype_t type;
	int start;
	int end;
	int skip;
#ifdef JSMN_PARENT_LINKS
	int parent;
#endif
} jsmntok_t;

/**
 * JSON parser. Contains an array of token blocks available. Also stores
 * the string being parsed now and current position in that string
 */
typedef struct {
	unsigned int pos; /* offset in the JSON string */
	unsigned int toknext; /* next token to allocate */
	int toksuper; /* superior token node, e.g parent object or array */
} jsmn_parser;

/**
 * Create JSON parser over an array of tokens
 */
void jsmn_init(jsmn_parser *parser);

/**
 * Run JSON parser. It parses a JSON data string into and array of tokens, each describing
 * a single JSON object.
 */
int jsmn_parse(jsmn_parser *parser, const char *js, size_t len,
		jsmntok_t *tokens, unsigned int num_tokens);

#ifdef __cplusplus
}
#endif

#endif /* __JSMN_H_ */
