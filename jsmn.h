#ifndef __JSMN_H_
#define __JSMN_H_

typedef enum {
	JSON_OBJECT,
	JSON_ARRAY,
	JSON_STRING,
	JSON_OTHER
} jsontype_t;

typedef struct {
	jsontype_t type;
	int start;
	int end;
} jsontok_t;

#endif /* __JSMN_H_ */
