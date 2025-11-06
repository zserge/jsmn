#include "../jsmn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct key_value_pair {
  char key[256];
  char *value;
  int value_len;
  jsmntype_t value_type;
};

// move to next key on the same level as the index
// this iter function will not dive into nested objects/arrays
static void next_primary_key(const jsmntok_t *tokens, int *index_ptr,
                             int num_tokens) {
  for (int char_end = tokens[*index_ptr].end;
       *index_ptr < num_tokens && tokens[*index_ptr].start < char_end;
       (*index_ptr)++)
    ;
}

static void type_to_str(jsmntype_t type, char *buf, size_t buf_size) {
  switch (type) {
  case JSMN_UNDEFINED:
    snprintf(buf, buf_size, "undefined");
    break;
  case JSMN_OBJECT:
    snprintf(buf, buf_size, "object");
    break;
  case JSMN_ARRAY:
    snprintf(buf, buf_size, "array");
    break;
  case JSMN_STRING:
    snprintf(buf, buf_size, "string");
    break;
  case JSMN_PRIMITIVE:
    snprintf(buf, buf_size, "primitive");
    break;
  default:
    snprintf(buf, buf_size, "unknown");
    break;
  }
}

// get the key value pair at the current index and move the index forward
void *walk(const char *json, const jsmntok_t *tokens, int *index_ptr,
           int num_tokens, struct key_value_pair *kv,
           void (*iter)(const jsmntok_t *tok, int *i, int r)) {
  if (*index_ptr >= num_tokens || *index_ptr < 0) {
    return NULL;
  }

  if (tokens[*index_ptr].type != JSMN_STRING) {
    return NULL;
  }

  int key_len = tokens[*index_ptr].end - tokens[*index_ptr].start;
  snprintf(kv->key, key_len + 1, "%.*s", key_len,
           json + tokens[*index_ptr].start);

  // move to value
  (*index_ptr)++;

  kv->value_len = tokens[*index_ptr].end - tokens[*index_ptr].start;
  if (kv->value_len <= 0) {
    return NULL;
  }
  kv->value_type = tokens[*index_ptr].type;
  kv->value = (char *)malloc(kv->value_len + 1);
  strncpy(kv->value, json + tokens[*index_ptr].start, kv->value_len);
  kv->value[kv->value_len] = '\0';

  // we can use custom iter function to skip nested objects/arrays
  if (iter != NULL) {
    iter(tokens, index_ptr, num_tokens);
  } else {
    // default behavior: just move to next token
    (*index_ptr)++;
  }

  return index_ptr;
}

static const char *JSON_STRING =
    "{\"user\": \"johndoe\",\n "
    "\"nested\": {\"info\": {\"age\": 30, \"city\": \"New York\"},\n "
    "\"next\": \"value\"}, \"primary\": \"primary_value\"}";

int main() {
  int r;
  jsmn_parser p;
  jsmntok_t t[128];

  jsmn_init(&p);
  r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t,
                 sizeof(t) / sizeof(t[0]));
  if (r < 0) {
    printf("Failed to parse JSON: %d\n", r);
    return 1;
  }

  // top level check
  if (r < 1 || t[0].type != JSMN_OBJECT) {
    printf("object expected\n");
    return 1;
  }

  struct key_value_pair *kv =
      (struct key_value_pair *)malloc(sizeof(struct key_value_pair));

  int index = 1;
  char buf[256];

  while (walk(JSON_STRING, t, &index, r, kv, next_primary_key) != NULL) {
    type_to_str(kv->value_type, buf, sizeof(buf));
    printf("key: %s, value: %s, type: %s\n", kv->key, kv->value, buf);
    free(kv->value);
  }
  free(kv);
}
