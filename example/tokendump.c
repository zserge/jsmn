//#define JSMN_TOKNEXT

#include "../jsmn.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void dump(char *js, jsmntok_t* tokens, int tokenCount) {
  puts("index    type[size] start-end toknext value");
  puts("-------------------------------------------");
  for (int ti = 0; ti < tokenCount; ti++) {
    jsmntok_t t = tokens[ti];
    char *type;
    switch (t.type) {
    case JSMN_ARRAY:
      type = "ARRAY";
      break;
    case JSMN_OBJECT:
      type = "OBJECT";
      break;
    case JSMN_PRIMITIVE:
      type = "PRIMITIVE";
      break;
    case JSMN_STRING:
      type = "STRING";
      break;
    }
    printf("%3d: %9s[%03d]: %4d-%4d: %4d", ti, type, t.size, t.start, t.end,
           jsmn_next_token(tokens, tokenCount, ti));
    switch (t.type) {
    case JSMN_PRIMITIVE:
    case JSMN_STRING:
      printf(": ");
      for (int i = t.start; i < t.end;)
        putchar(js[i++]);
    }
    putchar('\n');
  }
}

void help() {
  puts("tokendump <json>");
  exit(1);
}

int main(int argc, char *args[]) {
  if (argc != 2)
    help();
  FILE *f = fopen(args[1], "r");
  if (f == NULL)
    help();

  // load file to memory
  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *js = malloc(size + 1);
  for (int i = 0; i < size;)
    js[i++] = fgetc(f);
  js[size] = 0;
  fclose(f);

  // count tokens first
  jsmn_parser parser;
  jsmn_init(&parser);
  int len = strlen(js);
  int tokenCount = jsmn_parse(&parser, js, len, NULL, -1);
  if (tokenCount > 0) {
    // parse them
    jsmntok_t *tokens = malloc(tokenCount * sizeof(jsmntok_t));
    jsmn_init(&parser);
    jsmn_parse(&parser, (const char *)js, len, tokens, tokenCount);
    // and dump finally
    dump(js, tokens, tokenCount);
  }
}
