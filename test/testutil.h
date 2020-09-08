#ifndef __TEST_UTIL_H__
#define __TEST_UTIL_H__

#include "../jsmn.h"

static int vtokeq(const char *s, const jsmntok_t *t, const size_t numtok,
                  va_list ap) {
  if (numtok > 0) {
    size_t i;
    jsmnint_t start, end, size;
    jsmntype_t type;
    char *value;

    size = JSMN_NEG;
    value = NULL;
    for (i = 0; i < numtok; i++) {
      type = va_arg(ap, jsmntype_t);
      if (type == JSMN_STRING) {
        value = va_arg(ap, char *);
        size = va_arg(ap, int);
        start = end = JSMN_NEG;
      } else if (type == JSMN_PRIMITIVE) {
        value = va_arg(ap, char *);
        start = end = size = JSMN_NEG;
      } else {
        start = va_arg(ap, int);
        end = va_arg(ap, int);
        size = va_arg(ap, int);
        value = NULL;
      }
      if (!(t[i].type & type)) {
        printf("token %lu type is %d, not %d\n", i, t[i].type, type);
        return 0;
      }
      if (start != JSMN_NEG && end != JSMN_NEG) {
        if (t[i].start != start) {
          printf("token %lu start is %d, not %d\n", i, t[i].start, start);
          return 0;
        }
        if (t[i].end != end) {
          printf("token %lu end is %d, not %d\n", i, t[i].end, end);
          return 0;
        }
      }
      if (size != JSMN_NEG && t[i].size != size) {
        printf("token %lu size is %d, not %d\n", i, t[i].size, size);
        return 0;
      }

      if (s != NULL && value != NULL) {
        const char *p = s + t[i].start;
        if (strlen(value) != (size_t)(t[i].end - t[i].start) ||
            strncmp(p, value, t[i].end - t[i].start) != 0) {
          printf("token %lu value is %.*s, not %s\n", i, t[i].end - t[i].start,
                 s + t[i].start, value);
          return 0;
        }
      }
    }
  }
  return 1;
}

static int tokeq(const char *s, const jsmntok_t *tokens, const size_t numtok, ...) {
  int ok;
  va_list args;
  va_start(args, numtok);
  ok = vtokeq(s, tokens, numtok, args);
  va_end(args);
  return ok;
}

static int query(const char *s, const jsmnint_t status) {
    jsmnint_t r;
    jsmn_parser p;

    jsmn_init(&p);
    r = jsmn_parse(&p, s, strlen(s), NULL, 0);

    return (status == r);
}

static int parse(const char *s, const jsmnint_t status, const size_t numtok, ...) {
  jsmnint_t r;
  int ok = 1;
  va_list args;
  jsmn_parser p;
  jsmntok_t *t = malloc(numtok * sizeof(jsmntok_t));

  jsmn_init(&p);
  r = jsmn_parse(&p, s, strlen(s), t, numtok);
  if (r != status) {
    printf("status is %d, not %d\n", r, status);
    return 0;
  }

  if (status >= 0) {
    va_start(args, numtok);
    ok = vtokeq(s, t, numtok, args);
    va_end(args);
  }
  free(t);
  return ok;
}

#endif /* __TEST_UTIL_H__ */
