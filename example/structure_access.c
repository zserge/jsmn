#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "../jsmn.h"
#include <assert.h>

/* Function realloc_it() is a wrapper function for standard realloc()
 * with one difference - it frees old memory pointer in case of realloc
 * failure. Thus, DO NOT use old data pointer in anyway after call to
 * realloc_it(). If your code has some kind of fallback algorithm if
 * memory can't be re-allocated - use standard realloc() instead.
 */
static inline void *realloc_it(void *ptrmem, size_t size) {
    void *p = realloc(ptrmem, size);
    if (!p)  {
        free (ptrmem);
        fprintf(stderr, "realloc(): errno=%d\n", errno);
    }
    return p;
}

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
    if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
            strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}

//  B
int get_primitive(const char *js, jsmntok_t *t, int i) {
            int j;
            if (t[i].type != JSMN_STRING) {
                assert(0); 
            }
            jsmntok_t *g = t + i;
          printf("  * %.*s\n", g->end - g->start, js + g->start);
      return i;
}

//  A:B
int get_key_val(const char *js, jsmntok_t *t, int i) {
            if (t[i].type != JSMN_STRING) {
                assert(0); 
            }
      i = get_primitive(js, t, i);

      i++;
      i = get_primitive(js, t, i);

      return i;
}


//  [B1, B2]
int get_array_of_primitives(const char *js, jsmntok_t *t, int i) {
            int j;
            if (t[i].type != JSMN_ARRAY) {
                assert(0); 
            }
      int size = t[i].size;
      i++;
            for (j = 0; j < size  - 1; j++) {
        i = get_primitive(js, t, i);
        i++;
            }
      i = get_primitive(js, t, i);
      return i;
}

//  {B:C, D:E}
int get_struct(const char *js, jsmntok_t *t, int i) {
            int j;
            if (t[i].type != JSMN_OBJECT) {
                assert(0); 
            }

      int size = t[i].size;
      i++;

            for (j = 0; j < size - 1; j++) {
        i = get_key_val(js, t, i);
        i++;
            }
      i = get_key_val(js, t, i);
      return i;
}

//    [
//     {B:C}, 
//      {D:E}
//    ]
int get_aray_of_struct(const char *js, jsmntok_t *t, int i) {
            int j;
            if (t[i].type != JSMN_ARRAY) {
                assert(0); 
            }

      int size = t[i].size;
      i++;

            for (j = 0; j < size -1; j++) {
        i = get_struct(js, t, i);
        i++;
            }
      i = get_struct(js, t, i);
      return i;
}

int get_struct_of_array(const char *js, jsmntok_t *t, int i) {
            int j;
            if (t[i].type != JSMN_OBJECT) {
                assert(0); 
            }

      int size = t[i].size;
      i++;

            for (j = 0; j < size -1; j++) {
        i = get_primitive(js, t, i);
        i++;
        i = get_array_of_primitives(js, t, i);
        i++;
            }
      i = get_primitive(js, t, i);
      i++;
      i = get_array_of_primitives(js, t, i);
      return i;


}

int get_K6(const char *js, jsmntok_t *t, int i) {
            int j;
            if (t[i].type != JSMN_ARRAY) {
                assert(0); 
            }

      int size = t[i].size;
      i++;

            for (j = 0; j < size -1; j++) {
        i = get_struct_of_array(js, t, i);
        i++;
            }
      i = get_struct_of_array(js, t, i);
      return i;
  
}

static void dump(const char *js, jsmntok_t *t, int r) {
    /* Loop over all keys of the root object */
    for (int i = 1; i < r; i++) {
    
        if (jsoneq(js, &t[i], "K1") == 0) {
      i++;
      i = get_primitive(js, t, i);
      continue;
        } 

        if (jsoneq(js, &t[i], "K2") == 0) {
      i++;
      i = get_array_of_primitives(js, t, i);
      continue;
        } 
    
        if (jsoneq(js, &t[i], "K3") == 0) {
      i++;
      i = get_struct(js, t, i);
      continue;
        } 

        if (jsoneq(js, &t[i], "K4") == 0) {
      i++;
      i = get_aray_of_struct(js, t, i);
      continue;
        } 

        if (jsoneq(js, &t[i], "K5") == 0) {
      i++;
      i = get_primitive(js, t, i);
      continue;
        } 

        if (jsoneq(js, &t[i], "K6") == 0) {
      i++;
      i = get_K6(js, t, i);
      continue;
        } 
      printf("Unexpected key: %.*s\n", t[i].end-t[i].start,
                    js + t[i].start);
    }
  
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
    tok = (jsmntok_t *)malloc(sizeof(*tok) * tokcount);
    if (tok == NULL) {
        fprintf(stderr, "malloc(): errno=%d\n", errno);
        return 3;
    }

  FILE *fd = fopen("example/example1.json", "r");
  if(NULL == fd) {
        fprintf(stderr, "fopen(): errno=%d\n", errno);
        return 3;
  }

    for (;;) {
        /* Read another chunk */
        r = fread(buf, 1, sizeof(buf), fd);
        if (r < 0) {
            fprintf(stderr, "fread(): %d, errno=%d\n", r, errno);
            return 1;
        }
        if (r == 0) {
            if (eof_expected != 0) {
                free(js);
                free(tok);
                fclose(fd);
                return 0;
            } else {
                fprintf(stderr, "fread(): unexpected EOF\n");
                return 2;
            }
        }

        js = (char *)realloc_it(js, jslen + r + 1);
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
                tok = (jsmntok_t *)realloc_it(tok, sizeof(*tok) * tokcount);
                if (tok == NULL) {
                    return 3;
                }
                goto again;
            }
        } else {
            dump(js, tok, r);
            eof_expected = 1;
        }
    }

    return EXIT_SUCCESS;
}
