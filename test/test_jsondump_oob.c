#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * Regression test for jsmn issue #249.
 *
 * jsmn's non-strict grammar (the default, without -DJSMN_STRICT) accepts
 * some inputs that are not valid JSON. One consequence is that a token
 * can end up with a "size" that does not match how many children are
 * actually stored after it in the token array. The example shipped in
 * example/jsondump.c trusted that size unconditionally and recursed
 * through the token tree using it, which let a crafted, malformed
 * document walk dump() straight past the end of the token array (an
 * AddressSanitizer heap-buffer-overflow at jsondump.c, reachable simply
 * by piping the input into the unmodified example).
 *
 * This test reuses the actual dump() from example/jsondump.c (not a
 * copy of it) so that a regression in the real bounds check gets
 * caught here.
 */
#define main jsondump_main
#include "../example/jsondump.c"
#undef main

/*
 * Builds a deeply nested, deliberately malformed document: a run of
 * ordinary `{"a":` openings, a spot where a `:` is missing after a key
 * and a stray `"` follows shortly after, more nesting, a primitive
 * value, and a matching run of closing braces. jsmn's non-strict
 * grammar parses this without returning an error, but the resulting
 * token tree does not actually hold as many children as the object
 * tokens near the malformed spot claim.
 */
static char *build_oob_input(size_t *out_len) {
  const char *open_key = "{\"a\":";
  const char *malformed = "{\"a\"{\"a\":{\"a\":{\"a\":{\"a\":{\"\"a\":";
  const char *value = "null";
  const int leading_depth = 102;
  const int trailing_depth = 147;
  const int closing_braces = 255;
  size_t open_key_len = strlen(open_key);
  size_t malformed_len = strlen(malformed);
  size_t value_len = strlen(value);
  size_t total = open_key_len * (size_t)leading_depth + malformed_len +
                 open_key_len * (size_t)trailing_depth + value_len +
                 (size_t)closing_braces;
  char *buf = malloc(total + 1);
  char *p = buf;
  int i;

  if (buf == NULL) {
    return NULL;
  }

  for (i = 0; i < leading_depth; i++) {
    memcpy(p, open_key, open_key_len);
    p += open_key_len;
  }
  memcpy(p, malformed, malformed_len);
  p += malformed_len;
  for (i = 0; i < trailing_depth; i++) {
    memcpy(p, open_key, open_key_len);
    p += open_key_len;
  }
  memcpy(p, value, value_len);
  p += value_len;
  memset(p, '}', (size_t)closing_braces);
  p += closing_braces;
  *p = '\0';

  *out_len = (size_t)(p - buf);
  return buf;
}

int main(void) {
  size_t len;
  char *js = build_oob_input(&len);
  jsmn_parser p;
  jsmntok_t *tok;
  size_t tokcount = 2;
  int r;
  int devnull, saved_stdout;

  if (js == NULL) {
    fprintf(stderr, "FAILED: could not build test input\n");
    return 1;
  }

  jsmn_init(&p);
  tok = malloc(sizeof(*tok) * tokcount);
  if (tok == NULL) {
    fprintf(stderr, "FAILED: out of memory\n");
    free(js);
    return 1;
  }

  for (;;) {
    r = jsmn_parse(&p, js, len, tok, tokcount);
    if (r == JSMN_ERROR_NOMEM) {
      tokcount *= 2;
      tok = realloc(tok, sizeof(*tok) * tokcount);
      if (tok == NULL) {
        fprintf(stderr, "FAILED: out of memory\n");
        free(js);
        return 1;
      }
      continue;
    }
    break;
  }

  if (r < 0) {
    /* jsmn rejected the input outright, so dump() never gets a
     * malformed token tree to walk; nothing to regress on this run. */
    free(tok);
    free(js);
    printf("PASSED: input was rejected by jsmn_parse (nothing to dump)\n");
    return 0;
  }

  /* dump()'s recursive descent below used to read past the end of tok[]
   * on this input. Its own output isn't interesting here, so it is
   * sent to /dev/null; reaching the PASSED line at all is the point. */
  fflush(stdout);
  devnull = open("/dev/null", O_WRONLY);
  saved_stdout = dup(STDOUT_FILENO);
  if (devnull >= 0 && saved_stdout >= 0) {
    dup2(devnull, STDOUT_FILENO);
  }

  dump(js, tok, p.toknext, 0);

  fflush(stdout);
  if (saved_stdout >= 0) {
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);
  }
  if (devnull >= 0) {
    close(devnull);
  }

  free(tok);
  free(js);
  printf("PASSED: dump() survived a malformed non-strict-mode input\n");
  return 0;
}
