/***********************************************************************
 * An example of reading JSON from stdin or filename, storing contents in
 * a buffer, parsing the buffer, and thenprinting its content * to stdout.
 * The output contains only primitives and the lengths of any arrays,
 * preceded by their Javascript-ish name; the name starts with "json"
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "../jsmn.h"
#include "buffer_file.c"


/***********************************************************************
 * dump(...) - recursive printer of JSON primitives that walks though
 *             JSMN tokens
 * Arguments:
 *   js - pointer to memory buffer with JSON
 *   t - pointer JSMN tokens; t[I].start and t[I].end are indices into js
 *   count - number of tokens remaining
 *   indent - indentation level
 *   idpfx - name of the object or array container
 */
static int dump(const uint8_t* js, jsmntok_t *t, size_t count, int indent, char idpfx[BUFSIZ]) {
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
      /* Append object Javascript-ish name to idpfx buffer,
       * IFF it will not overflow idbpx
       */
      idOk = idpfx && ((idpfxpos + 1 + (t[1+j].end - t[1+j].start)) < BUFSIZ);
      if (idOk) { sprintf(idpfxend, ".%.*s", t[1+j].end - t[1+j].start, js+t[1+j].start); }
      /* Process member name */
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


/***********************************************************************
 * Main program:  read JSON file into memory; parse JSON; dump primitives
 */
int main(int argc, char** argv) {
int r;
size_t jslen = 0;
char* filename = argc > 1 ? argv[1] : "-";
uint8_t* js = (uint8_t*) buffile_file_to_puint8(filename, &jslen, 0);
char idpfx[BUFSIZ] = { "json" };
jsmn_parser p;
size_t tokcount = 64;
jsmntok_t* tok = realloc(0, sizeof(jsmntok_t) * tokcount);
int rtn = 3;

# define BRK(FPARGS, RTN) fprintf FPARGS; rtn = RTN

  /* Initialize parser; allocate some tokens as a start */
  jsmn_init(&p);

  /* Parse JSON, re-allocating double the # of token structs as needed */
  while (js && tok && JSMN_ERROR_NOMEM == (r=jsmn_parse(&p, js, jslen, tok, tokcount))) {
  jsmntok_t *old_tok;
    old_tok = tok;
    tok = realloc(old_tok, sizeof(jsmntok_t) * (tokcount <<= 1));
    if (!tok) { free(old_tok); break; }
  }

  if (!js) { BRK((stderr, "buffile_file_to_puint8(...) failed to read file into memory buffer\n"), 1); }
  else if (!tok) { BRK((stderr, "realloc() failed to allocation tokens\n"), 2); }
  else if (r == JSMN_ERROR_INVAL) { BRK((stderr, "jsmn_parse() error e.g. invalid character\n"), 3); }
  else if (r == JSMN_ERROR_PART) { BRK((stderr, "jsmn_parse() error; incomplete JSON\n"), 4); }
  else if (r == 0) { BRK((stderr, "jsmn_parse() error; possible empty JSON file\n"), 4); }
  else if (r > 0) { BRK( , EXIT_SUCCESS); }
  else { BRK((stderr, "jsmn_parse() unknown error\n"), 5); }

  /* Dump token contents IFF parse was successful*/
  if (rtn == EXIT_SUCCESS) { dump(js, tok, p.toknext, 0, idpfx); }

  /* Clean up and return */
  if (tok) free(tok);
  if (js) free(js);
  return rtn;
}
