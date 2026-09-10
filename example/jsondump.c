#include "../jsmn.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Function realloc_it() is a wrapper function for standard realloc()
 * with one difference - it frees old memory pointer in case of realloc
 * failure. Thus, DO NOT use old data pointer in anyway after call to
 * realloc_it(). If your code has some kind of fallback algorithm if
 * memory can't be re-allocated - use standard realloc() instead.
 */
static inline void *realloc_it(void *ptrmem, size_t size) {
  void *p = realloc(ptrmem, size);
  if (!p) {
    free(ptrmem);
    fprintf(stderr, "realloc(): errno=%d\n", errno);
  }
  return p;
}

/*
 * An example of reading JSON from stdin and printing its content to stdout.
 * The output looks like YAML, but I'm not sure if it's really compatible.
 */

static int dump(const char *js, jsmntok_t *t, size_t count, int indent) {
  int i, j, k;
  jsmntok_t *key;
  if (count == 0) {
    return 0;
  }
  if (t->type == JSMN_PRIMITIVE) {
    printf("%.*s", t->end - t->start, js + t->start);
    return 1;
  } else if (t->type == JSMN_STRING) {
    printf("'%.*s'", t->end - t->start, js + t->start);
    return 1;
  } else if (t->type == JSMN_OBJECT) {
    printf("\n");
    j = 0;
    for (i = 0; i < t->size; i++) {
      /* count still includes t itself, so a child at offset 1 + j only
       * exists while 1 + j is less than count. jsmn's non-strict grammar
       * can accept malformed input whose token tree claims more children
       * for t than the token pool actually holds (see jsmn issue #249),
       * so this has to be checked before the key pointer is even formed,
       * let alone dereferenced. */
      if ((size_t)(1 + j) >= count) {
        break;
      }
      for (k = 0; k < indent; k++) {
        printf("  ");
      }
      key = t + 1 + j;
      j += dump(js, key, count - 1 - j, indent + 1);
      if ((size_t)(1 + j) < count && key->size > 0) {
        printf(": ");
        j += dump(js, t + 1 + j, count - 1 - j, indent + 1);
      }
      printf("\n");
    }
    return j + 1;
  } else if (t->type == JSMN_ARRAY) {
    j = 0;
    printf("\n");
    for (i = 0; i < t->size; i++) {
      if ((size_t)(1 + j) >= count) {
        break;
      }
      for (k = 0; k < indent - 1; k++) {
        printf("  ");
      }
      printf("   - ");
      j += dump(js, t + 1 + j, count - 1 - j, indent + 1);
      printf("\n");
    }
    return j + 1;
  }
  return 0;
}

int main() {
  int r;
  int eof_expected = 0;
  char *js = NULL;
  size_t jslen = 0;
  char buf[BUFSIZ];

  jsmn_parser p;
  jsmntok_t *tok;
  size_t tokcount = 2;

  /* Prepare parser */
  jsmn_init(&p);

  /* Allocate some tokens as a start */
  tok = malloc(sizeof(*tok) * tokcount);
  if (tok == NULL) {
    fprintf(stderr, "malloc(): errno=%d\n", errno);
    return 3;
  }

  for (;;) {
    /* Read another chunk */
    r = fread(buf, 1, sizeof(buf), stdin);
    if (r < 0) {
      fprintf(stderr, "fread(): %d, errno=%d\n", r, errno);
      return 1;
    }
    if (r == 0) {
      if (eof_expected != 0) {
        return 0;
      } else {
        fprintf(stderr, "fread(): unexpected EOF\n");
        return 2;
      }
    }

    js = realloc_it(js, jslen + r + 1);
    if (js == NULL) {
      return 3;
    }
    strncpy(js + jslen, buf, r);
    jslen = jslen + r;

  again:
    r = jsmn_parse(&p, js, jslen, tok, tokcount);
    if (r < 0) {
      if (r == JSMN_ERROR_NOMEM) {
        tokcount = tokcount * 2;
        tok = realloc_it(tok, sizeof(*tok) * tokcount);
        if (tok == NULL) {
          return 3;
        }
        goto again;
      }
    } else {
      dump(js, tok, p.toknext, 0);
      eof_expected = 1;
    }
  }

  return EXIT_SUCCESS;
}
