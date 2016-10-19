#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "test.h"
#include "testutil.h"

#include "../jsmn_iterator.c"

#ifndef countof
#define countof(_x_) (sizeof(_x_)/sizeof((_x_)[0]))
#endif

#define MULTILINE(...) #__VA_ARGS__

/**
 * @brief Validate iterator
 * @details Uses the iterator to walk the JSON tree and validates that all items is visited in the right order
 * 
 * @param jsmn_tokens [description]
 * @param jsmn_len [description]
 * 
 * @return [description]
 */
int iterator_validate(jsmntok_t *jsmn_tokens, unsigned int jsmn_len) {

  jsmn_iterator_t stack[1024];

  jsmntok_t *jsmn_value = NULL;
  jsmntok_t *jsmn_identifier = NULL;

  unsigned int jsmn_iterator_index = 1;
  unsigned int stack_index = 0;
  unsigned int iterator_hint = 0;
  int exit_line = 0;
  int ret_value;

#define JSMN_INDEX(_item_) (unsigned int)(_item_ - jsmn_tokens)
#define BAIL_OUT() do { exit_line = __LINE__; goto error_out; } while(0)

  /* Setup root item */
  stack_index = 0;
  if ((ret_value = jsmn_iterator_init(&stack[stack_index], jsmn_tokens, jsmn_len, 0)) < 0) {
    BAIL_OUT();
  }

  /* Set hint to first item */
  iterator_hint = 1;

  /* Index for first item */
  jsmn_iterator_index = 1;

  /* Iterate over all items, only abort on error. We don't pass iterator_hint which means 
     that the iterator will fallback to internal function to determine where next object begins*/
  while ((ret_value = jsmn_iterator_next(&stack[stack_index], &jsmn_identifier, &jsmn_value, 0)) >= 0) {
    /* No more items, need to pop stack if possible */
    if (ret_value == 0) {
      /* Back at root item, so bail out */
      if (stack_index == 0)
        break;

      /* parser_pos should point at the index after the current Array/Object,
         we use it to hint to the outer Array/Object where next item is */
      iterator_hint = stack[stack_index].parser_pos;

      /* Pop the stack */
      stack_index--;

      /* Continue using new stack pointer */
      continue;
    }

    /* Validate that iterator hint is correct (need to compensate for Object) */
    if (iterator_hint != JSMN_INDEX(jsmn_value) - (jsmn_identifier ? 1 : 0))
      BAIL_OUT();

    /* We have identifier so we are inside object, add this before the value */
    if (jsmn_identifier) {
      if (jsmn_iterator_index != JSMN_INDEX(jsmn_identifier))
        BAIL_OUT();
      jsmn_iterator_index++;
    }

    if (jsmn_iterator_index != JSMN_INDEX(jsmn_value))
      BAIL_OUT();
    jsmn_iterator_index++;

    /* Always put iterator_hint to the next value, if end of Array/Object it will
       be updated when popping the stack */
    iterator_hint = JSMN_INDEX(jsmn_value) + 1;

    /* Need to handle sublevels before next object */
    if (jsmn_value->type == JSMN_OBJECT || jsmn_value->type == JSMN_ARRAY) {
      if (stack_index + 1 >= countof(stack))
        BAIL_OUT();

      /* Initiate new iterator on the stack */
      if (jsmn_iterator_init(&stack[stack_index + 1], jsmn_tokens, jsmn_len, stack[stack_index].parser_pos) < 0)
        BAIL_OUT();

      /* Update stack pointer */
      stack_index++;
    }
  }

  /* Check for error abort of while loop */
  if (ret_value != 0)
    BAIL_OUT();

  /* Check so we have visited all items */
  if (jsmn_iterator_index != jsmn_len)
    BAIL_OUT();

  return 0;
error_out:
  printf("exit_line: %d\r\n", exit_line);

  if (jsmn_iterator_index != jsmn_len) {
    return (int)jsmn_iterator_index;
  }

  return -100;
}


int validate_json(const char *js) {
  int r;
  jsmn_parser p;
  static jsmntok_t tokens[2048];
  jsmn_init(&p);
  r = jsmn_parse(&p, js, strlen(js), tokens, countof(tokens));
  check(r >= 0);

  return iterator_validate(tokens, (unsigned int)r);
}



int test_issue_22(void) {
  const char *js;
  js = MULTILINE(
    {
      "height": 10,
      "layers": [
        {
          "data": [
            6,
            6
          ],
          "height": 10,
          "name": "Calque de Tile 1",
          "opacity": 1,
          "type": "tilelayer",
          "visible": true,
          "width": 10,
          "x": 0,
          "y": 0
        }
      ],
      "orientation": "orthogonal",
      "properties": { },
      "tileheight": 32,
      "tilesets": [
        {
          "firstgid": 1,
          "image": "..\\/images\\/tiles.png",
          "imageheight": 64,
          "imagewidth": 160,
          "margin": 0,
          "name": "Tiles",
          "properties": { },
          "spacing": 0,
          "tileheight": 32,
          "tilewidth": 32
        }
      ],
      "tilewidth": 32,
      "version": 1,
      "width": 10
    });
  return validate_json(js);
}

int test_json_schema_org_example1(void) {
  const char *js;

  js = MULTILINE(
    {
      "$schema": "http://json-schema.org/draft-04/schema#",
      "title": "Product set",
      "type": "array",
      "items": {
          "title": "Product",
          "type": "object",
          "properties": {
              "id": {
                  "description": "The unique identifier for a product",
                  "type": "number"
              },
              "name": {
                  "type": "string"
              },
              "price": {
                  "type": "number",
                  "minimum": 0,
                  "exclusiveMinimum": true
              },
              "tags": {
                  "type": "array",
                  "items": {
                      "type": "string"
                  },
                  "minItems": 1,
                  "uniqueItems": true
              },
              "dimensions": {
                  "type": "object",
                  "properties": {
                      "length": {"type": "number"},
                      "width": {"type": "number"},
                      "height": {"type": "number"}
                  },
                  "required": ["length", "width", "height"]
              },
              "warehouseLocation": {
                  "description": "Coordinates of the warehouse with the product",
                  "$ref": "http://json-schema.org/geo"
              }
          },
          "required": ["id", "name", "price"]
      }
    }
  );

  return validate_json(js);
}

int test_json_schema_org_example2(void) {
  const char *js;

  js = MULTILINE(
    {
      "id": "http://some.site.somewhere/entry-schema#",
      "$schema": "http://json-schema.org/draft-04/schema#",
      "description": "schema for an fstab entry",
      "type": "object",
      "required": [ "storage" ],
      "properties": {
          "storage": {
              "type": "object",
              "oneOf": [
                  { "$ref": "#/definitions/diskDevice" },
                  { "$ref": "#/definitions/diskUUID" },
                  { "$ref": "#/definitions/nfs" },
                  { "$ref": "#/definitions/tmpfs" }
              ]
          },
          "fstype": {
              "enum": [ "ext3", "ext4", "btrfs" ]
          },
          "options": {
              "type": "array",
              "minItems": 1,
              "items": { "type": "string" },
              "uniqueItems": true
          },
          "readonly": { "type": "boolean" }
      },
      "definitions": {
          "diskDevice": {},
          "diskUUID": {},
          "nfs": {},
          "tmpfs": {}
      }
    }
  );

  return validate_json(js);
}

int test_deep(void) {
  const char *js;

  js = MULTILINE(
    {"a":{"a":{"a":[{"a":[{"a":[{"a":[{"a":[{"a":[{"a":[{"a":[[{"a":[{"a":[
    {"a":{"a":{"a":[{"a":[{"a":[{"a":[{"a":[{"a":[{"a":[{"a":[[{"a":[{"a":[
    {"a":{"a":{"a":[{"a":[{"a":[{"a":[{"a":[{"a":[{"a":[{"a":[[{"a":[{"a":[
    {"a":[{"a":[{"a":[{"a":[{"a":[{"a":[{"a":[{"a":[
    1
    ]}]}]}]}]}]}]}]}
    ]}]}]]}]}]}]}]}]}]}]}}}
    ]}]}]]}]}]}]}]}]}]}]}}}
    ]}]}]]}]}]}]}]}]}]}]}}}
  );

  return validate_json(js);
}

int coverage_jsmn_iterator_find_last(void) {
  int exit_line = 0;

  jsmntok_t jsmn_tokens[128];
  jsmn_parser p;
  int jsmn_token_count;

  const char *js;

  // Setup some jsmn tokens
  jsmn_init(&p);
  js = MULTILINE(
    {
      "A": "B",
      "C": "D",
      "E": {
        "F": "G"
      },
      "H": [
        "I",
        "J"
      ]
    }
  );
  if ((jsmn_token_count = jsmn_parse(&p, js, strlen(js), jsmn_tokens, 128)) < 0)
    BAIL_OUT();

  // Null arguments
  if (jsmn_iterator_find_last(NULL, 0, 0) != JSMNITER_ERR_PARAMETER)
    BAIL_OUT();
  if (jsmn_iterator_find_last(jsmn_tokens, 0, 2) != JSMNITER_ERR_PARAMETER)
    BAIL_OUT();
  if (jsmn_iterator_find_last(jsmn_tokens, jsmn_token_count, jsmn_token_count+2) != JSMNITER_ERR_PARAMETER)
    BAIL_OUT();
  if (jsmn_iterator_find_last(jsmn_tokens, jsmn_token_count, 1) != JSMNITER_ERR_TYPE)
    BAIL_OUT();

  return 0;
error_out:
  printf("exit_line: %d\r\n", exit_line);
  return exit_line;
}


int coverage_jsmn_iterator_init(void) {
  int exit_line = 0;
  
  jsmntok_t jsmn_tokens[128];
  jsmn_parser p;
  jsmn_iterator_t iter;
  int jsmn_token_count;

  const char *js;

  // Setup some jsmn tokens
  jsmn_init(&p);
  js = MULTILINE(
    {
      "A": "B",
      "C": "D",
      "E": {
        "F": "G"
      },
      "H": [
        "I",
        "J"
      ]
    }
  );
  if ((jsmn_token_count = jsmn_parse(&p, js, strlen(js), jsmn_tokens, 128)) < 0)
    BAIL_OUT();

  // Null arguments
  if (jsmn_iterator_init(NULL, NULL, 0, 0) != JSMNITER_ERR_PARAMETER)
    BAIL_OUT();
  if (jsmn_iterator_init(&iter, NULL, 0, 0) != JSMNITER_ERR_PARAMETER)
    BAIL_OUT();
  if (jsmn_iterator_init(&iter, jsmn_tokens, 0, 2) != JSMNITER_ERR_PARAMETER)
    BAIL_OUT();
  if (jsmn_iterator_init(&iter, jsmn_tokens, jsmn_token_count, jsmn_token_count + 2) != JSMNITER_ERR_PARAMETER)
    BAIL_OUT();
  if (jsmn_iterator_init(&iter, jsmn_tokens, jsmn_token_count, 1) != JSMNITER_ERR_TYPE)
    BAIL_OUT();

  return 0;
error_out:
  printf("exit_line: %d\r\n", exit_line);
  return exit_line;
}


int coverage_jsmn_iterator_next(void) {
  int exit_line = 0;

  jsmntok_t jsmn_tokens[128];
  jsmn_parser p;
  jsmn_iterator_t iter;
  jsmntok_t *ident;
  jsmntok_t *value;
  int jsmn_token_count;

  const char *js;

  // Setup some jsmn tokens
  jsmn_init(&p);
  js = MULTILINE(
    {
      "A": [ "B" ],
      "B": [ "B" ],
      "C": [ "C" ],
      "D": [ "D" ],
      "E": [   // 14
        "E", // 15
        "E", // 16
        "E"  // 17
      ],
      "F": "F"
    }
  );


  // Setup parser
  if ((jsmn_token_count = jsmn_parse(&p, js, strlen(js), jsmn_tokens, 128)) < 0)
    BAIL_OUT();
  if (jsmn_iterator_init(&iter, jsmn_tokens, jsmn_token_count, 0) < 0)
    BAIL_OUT();

  // Null arguments
  if (jsmn_iterator_next(NULL, NULL, NULL, 0) != JSMNITER_ERR_PARAMETER)
    BAIL_OUT();
  if (jsmn_iterator_next(&iter, NULL, NULL, 0) != JSMNITER_ERR_PARAMETER)
    BAIL_OUT();
  if (jsmn_iterator_next(&iter, NULL, &value, 0) != JSMNITER_ERR_PARAMETER)
    BAIL_OUT();

  // "A" iterator->parser_pos == iterator->parent_pos
  if (jsmn_iterator_next(&iter, &ident, &value, 1) <= 0) 
    BAIL_OUT();
  // "B" next_value_index == 0 
  if (jsmn_iterator_next(&iter, &ident, &value, 0) <= 0) 
    BAIL_OUT();
  // "C" next_value_index > jsmn_len
  if (jsmn_iterator_next(&iter, &ident, &value, jsmn_token_count + 2) <= 0)
    BAIL_OUT();
  // "D" next_value_index <= iterator->parser_pos
  if (jsmn_iterator_next(&iter, &ident, &value, 1) <= 0)
    BAIL_OUT();
  // "E" current_item->end < jsmn_tokens[next_value_index - 1].end
  if (jsmn_iterator_next(&iter, &ident, &value, 15) <= 0)
    BAIL_OUT();
  // "F" current_item->end < jsmn_tokens[next_value_index - 1].end
  if (jsmn_iterator_next(&iter, &ident, &value, 17) <= 0)
    BAIL_OUT();
  // END
  if (jsmn_iterator_next(&iter, &ident, &value, 0) != 0)
    BAIL_OUT();
  if (jsmn_iterator_next(&iter, &ident, &value, 0) != 0)
    BAIL_OUT();

  return 0;
error_out:
  printf("exit_line: %d\r\n", exit_line);
  return exit_line;
}



int main(void) {
  test(coverage_jsmn_iterator_find_last,  "Coverage: jsmn_iterator_find_last");
  test(coverage_jsmn_iterator_init,       "Coverage: jsmn_iterator_init");
  test(coverage_jsmn_iterator_next,       "Coverage: jsmn_iterator_next");
  test(test_issue_22, "test issue #22");
  test(test_json_schema_org_example1, "test http://json-schema.org/example1.html");
  test(test_json_schema_org_example2, "test http://json-schema.org/example2.html");
  test(test_deep, "test deep nesting");
  printf("\nPASSED: %d\nFAILED: %d\n", test_passed, test_failed);
  return (test_failed > 0);
}
