#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "../jsmn.h"

/* Function realloc_it() is a wrapper function for standard realloc()
 * with one difference - it frees old memory pointer in case of realloc
 * failure. Thus, DO NOT use old data pointer in anyway after call to
 * realloc_it(). If your code has some kind of fallback algorithm if
 * memory can't be re-allocated - use standard realloc() instead.
 */
static inline void *realloc_it(void *ptrmem, size_t size) {
  void *p = realloc(ptrmem, size);
  if (!p)  {
    if (ptrmem) { free (ptrmem); }
    fprintf(stderr, "realloc(): errno=%d\n", errno);
  }
  return p;
}

/* An example of reading JSON from stdin or file and printing its content
 * to stdout.  The output contains only primitives and the lengths of any
 * arrays, preceded by their name; the name starts with "json"
 */
static int dump(const char *js, jsmntok_t *t, size_t count, int indent, char* idpfx) {
int idpfxpos = idpfx ? strlen(idpfx) : 0;
char* idpfxend = idpfx + idpfxpos;
int i, j, k;
int idOk;

  if (count == 0) { return 0; }

  if (t->type == JSMN_PRIMITIVE) {
    if (idpfx) printf("%s  %.*s\n", idpfx, t->end - t->start, js+t->start);
    return 1;

  } else if (t->type == JSMN_STRING) {
    if (idpfx) printf("%s  '%.*s'\n", idpfx, t->end - t->start, js+t->start);
    return 1;

  } else if (t->type == JSMN_OBJECT) {
    for (j = i = 0; i < t->size; i++) {
      idOk = idpfx && ((idpfxpos + 1 + (t[1+j].end - t[1+j].start)) < BUFSIZ);
#     if 0
      if (t[1+j].type != JSMN_STRING) {
        fprintf(stderr,"ERROR(name is not string)\n");
        return 0;
      }
#     endif
      if (idOk) { sprintf(idpfxend, ".%.*s", t[1+j].end - t[1+j].start, js+t[1+j].start); }
      j += dump(js, t+1+j, count-j, indent+1, (char*) 0);
      j += dump(js, t+1+j, count-j, indent+1, idOk ? idpfx : (char*) 0);
    }
    if (idpfx) { *idpfxend = '\0'; }
    return j+1;

  } else if (t->type == JSMN_ARRAY) {
  char sfx[30];
    for (j = i = 0; i < t->size; i++) {
      sprintf(sfx, "[%d]", i);
      idOk = idpfx && ((idpfxpos + strlen(sfx) < BUFSIZ));
      if (idOk) { sprintf(idpfxend, "%s", sfx); }
      j += dump(js, t+1+j, count-j, indent+1, idOk ? idpfx : (char*) 0);
    }
    if (idpfx) {
      *idpfxend = '\0';
      printf("%s.length  %d\n", idpfx, i);
    }
    return j+1;
  }
  return 0;
}

int main(int argc, char** argv) {
int r;
int eof_expected = 0;
char *js = NULL;
size_t jslen = 0;
char buf[BUFSIZ];
char idpfx[BUFSIZ] = { "json" };
FILE *fIn = argc>1 ? fopen(argv[1],"rb") : stdin;
jsmn_parser p;
jsmntok_t *tok;
size_t tokcount = 64;
int rtn = 3;

# define BRK(FPARGS, RTN) fprintf FPARGS; rtn = RTN; break

  /* Initialize parser; allocate some tokens as a start */
  jsmn_init(&p);
  tok = realloc_it(0, sizeof(jsmntok_t) * tokcount);

  while (tok && fIn) {

    /* Read next chunk; check for error; check for EOF */
    r = fread(buf, 1, sizeof(buf), fIn);
    if (ferror(fIn)) { BRK((stderr, "fread(): %d, errno=%d\n", r, errno), 1); }
    if (r==0 && feof(fIn)) {
      if (eof_expected) { BRK( , EXIT_SUCCESS); }
      BRK((stderr, "fread(): unexpected EOF\n"), 2);
    }

    /* Append chunk to json string <js>, allocating js string as needed */
    if (r > 0) {
      if ( !(js = realloc_it(js, jslen + r + 1)) ) { BRK( , 3); }
      strncpy(js + jslen, buf, r);
      jslen += r;
      js[jslen] = '\0';
    }

    /* Parse JSON, re-allocating double the # of token structs as needed */
    while (tok && JSMN_ERROR_NOMEM == (r=jsmn_parse(&p, js, jslen, tok, tokcount))) {
      tok = realloc_it(tok, sizeof(jsmntok_t) * (tokcount <<= 1));
    }
    if (!tok) { BRK( , 3); }
    if (r == JSMN_ERROR_INVAL) { BRK((stderr, "jsmn_parse() error\n"), 4); }
    if (r == JSMN_ERROR_PART) { continue; }  /* Expect more JSON data */
    if (r > -1) { eof_expected = 1; }         /* Completed JSON parse */
  }

  /* Dump token contents if  parse was successful*/
  if (rtn == EXIT_SUCCESS) { dump(js, tok, p.toknext, 0, idpfx); }

  /* Clean up and return */
  if (tok) free(tok);
  if (js) free(js);
  if (fIn && fIn != stdin) { fclose(fIn); }
  return rtn;
}
