#define JSMN_STRICT
#define JSMN_PARENT_LINKS
#define JSMN_PARENT_CNT
#include "../jsmn.h"
#include "jsmn_tools.h"
#include <string.h>
#include <stdio.h>


int
jsmn_eq(const char *json, jsmntok_t *t, const char *s)
{
  if (t->type == JSMN_STRING && (int)strlen(s) == t->end - t->start &&
          strncmp(json + t->start, s, t->end - t->start) == 0) {
    return 0;
  }
  return -1;
}

jsmntok_t *
_next(jsmntok_t *t)
{
  return t + 1 + t->size ;
}

jsmntok_t *
jsmn_get_next(jsmntok_t *first, jsmntok_t *t)
{
  if (t == 0) {
    return first ;
  }
  if (t + t->size < first + first->size) {
    return _next(t) ;
  }
  return 0 ;
}

int
jsmn_object_enum (const char *js, jsmntok_t *t, size_t count,
                  jsmn_key_val_t * out, size_t out_count)
{
  jsmntok_t *start = t;
  jsmntok_t *key;
  jsmntok_t *val;

  if (t->type != JSMN_OBJECT) {
      printf("OBJECT EXPECTED!\r\n");
      return -1;
  }

  t = t + 1 ;

  while (t < start + count) {
    if (t->type == JSMN_STRING) {
      key = t ;
      val = t + 1 ;
      t += 1 ;

    } else if (t->type == JSMN_OBJECT) {
      printf("UNEXPECTED: OBJECT\r\n");
      return -1; ;
    } else if (t->type == JSMN_PRIMITIVE) {
      printf("UNEXPECTED: '%.*s'\r\n", t->end - t->start, js + t->start);
      return -1;
    } else {
      printf("UNEXPECTED:\r\n");
      return -1;
    }

#ifdef DEBUG
    printf("KEY: '%.*s': ", key->end - key->start, js + key->start);

    if ((val->type == JSMN_STRING) || (val->type == JSMN_PRIMITIVE)) {
        printf("VAL: '%.*s'\r\n", val->end - val->start, js + val->start);
    } else {
        printf("VAL: '%s\r\n", (val->type == JSMN_OBJECT) ? "OBJECT" : "ARRAY");
    }
#endif

    int i ;
    for (i=0; i<out_count; i++) {
      if (!out[i].value && (jsmn_eq(js, key, out[i].key) == 0)) {
         out[i].value = val ;
      }
    }

    t = _next (t) ;

  }

  return 0 ;
}

int jsmn_copy_str (const char *js, jsmntok_t *t, char * sz, size_t size)
{
  return snprintf (sz, size, "%.*s",  t->end - t->start, js + t->start) ;
}
