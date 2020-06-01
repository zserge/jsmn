/* vim: set expandtab ts=8 sts=2 sw=2:
 *
 * MIT License
 *
 * Copyright (c) 2010 Serge Zaitsev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef JSMN_H
#define JSMN_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef JSMN_STATIC
#define JSMN_API static
#else
#define JSMN_API extern
#endif

/**
 * JSON type identifier. Basic types are:
 * 	o Object
 * 	o Array
 * 	o String
 * 	o Other primitive: number, boolean (true/false) or null
 */
typedef enum {
  JSMN_UNDEFINED = 0,
  JSMN_OBJECT = 1,
  JSMN_ARRAY = 2,
  JSMN_STRING = 3,
  JSMN_PRIMITIVE = 4
} jsmntype_t;

enum jsmnerr {
  /* Not enough tokens were provided */
  JSMN_ERROR_NOMEM = -1,
  /* Invalid character inside JSON string */
  JSMN_ERROR_INVAL = -2,
  /* The string is not a full JSON packet, more bytes expected */
  JSMN_ERROR_PART = -3
};

/**
 * JSON token description.
 * type		type (object, array, string etc.)
 * start	start position in JSON data string
 * end		end position in JSON data string
 */
typedef struct jsmntok {
  jsmntype_t type;
  unsigned int start;
  unsigned int end;
  unsigned int size;
#ifdef JSMN_PARENT_LINKS
  int parent;
#endif
} jsmntok_t;

/**
 * JSON parser state. Describes what kind of token or character is expected
 * next.
 */
typedef enum {
  JSMN_DELIMITER = 0x1,   /* Expecting colon if JSMN_KEY, comma otherwise */
  JSMN_CAN_CLOSE = 0x2,   /* Object or array can close here */

  JSMN_KEY = 0x10,    /* Expecting an object key or the delimiter following */
  JSMN_VALUE = 0x20,  /* Expecting an object value or the delimiter following */
  JSMN_IN_ARRAY = 0x40,   /* Expecting array item or the delimiter following */
  /* In an object if (state & JSMN_IN_OBJECT). Otherwise, in an array or at
   * the root. */
  JSMN_IN_OBJECT = JSMN_KEY | JSMN_VALUE,

  /* Actual values for parser->state */
  /* Root: not in any array or object */
  JSMN_STATE_ROOT = 0x0,
  /* Just saw object opening ({). Expecting key or close. */
  JSMN_STATE_OBJ_NEW = (JSMN_KEY | JSMN_CAN_CLOSE),
  /* Just saw a comma in an object. Expecting key. */
  JSMN_STATE_OBJ_KEY = (JSMN_KEY),
  /* Just saw a key in an object. Expecting colon or maybe close/comma. */
  JSMN_STATE_OBJ_COLON = (JSMN_KEY | JSMN_DELIMITER),
  /* Just saw a colon in an object. Expecting value. */
  JSMN_STATE_OBJ_VAL = (JSMN_VALUE),
  /* Just saw a value in an object. Expecting comma or close. */
  JSMN_STATE_OBJ_COMMA = (JSMN_VALUE | JSMN_DELIMITER | JSMN_CAN_CLOSE),
  /* Just saw an array opening ([). Expecting item or close. */
  JSMN_STATE_ARRAY_NEW = (JSMN_IN_ARRAY | JSMN_CAN_CLOSE),
  /* Just saw a comma in an array. Expecting item. */
  JSMN_STATE_ARRAY_ITEM = (JSMN_IN_ARRAY),
  /* Just saw an item in an array. Expecting comma or close. */
  JSMN_STATE_ARRAY_COMMA = (JSMN_DELIMITER | JSMN_IN_ARRAY | JSMN_CAN_CLOSE)
} jsmnstate_t;

/**
 * JSON parser. Contains an array of token blocks available. Also stores
 * the string being parsed now and current position in that string.
 */
typedef struct jsmn_parser {
  unsigned int pos;     /* offset in the JSON string */
  unsigned int toknext; /* next token to allocate */
  int toksuper;         /* superior token node, e.g. parent object or array */
  jsmnstate_t state;    /* parser state, from jsmnstate_t */
  int tokbefore;        /* token immediately preceding the first token in the
                           current JSON object */
} jsmn_parser;

/**
 * Create JSON parser over an array of tokens
 */
JSMN_API void jsmn_init(jsmn_parser *parser);

/**
 * Run JSON parser. It parses a JSON data string into and array of tokens, each
 * describing
 * a single JSON object.
 */
JSMN_API int jsmn_parse(jsmn_parser *parser, const char *js, const size_t len,
                        jsmntok_t *tokens, const unsigned int num_tokens);

#ifndef JSMN_HEADER
/**
 * Allocates a fresh unused token from the token pool.
 */
static jsmntok_t *jsmn_alloc_token(jsmn_parser *parser, jsmntok_t *tokens,
                                   const size_t num_tokens) {
  jsmntok_t *tok;
  if (parser->toknext >= num_tokens) {
    return NULL;
  }
  tok = &tokens[parser->toknext++];
  tok->end = 0;
  tok->size = 0;
#ifdef JSMN_PARENT_LINKS
  tok->parent = parser->tokbefore;
#endif
  return tok;
}

/**
 * Fills token type and boundaries.
 */
static void jsmn_fill_token(jsmntok_t *token, const jsmntype_t type,
                            const int start, const int end) {
  token->type = type;
  token->start = start;
  token->end = end;
}

#ifdef JSMN_STRICT
static const char true_str[] = "true";
static const char false_str[] = "false";
static const char null_str[] = "null";

typedef enum {
  JSMN_NUM_INT_START,
  JSMN_NUM_INT_HAVE_SIGN,
  JSMN_NUM_INT_ZERO,
  JSMN_NUM_INT,
  JSMN_NUM_FRAC_START,
  JSMN_NUM_FRAC,
  JSMN_NUM_EXP_START,
  JSMN_NUM_EXP_HAVE_SIGN,
  JSMN_NUM_EXP
} jsmn_numberstate;
#endif

/**
 * If extend is false, fills next available token with JSON primitive. If
 * extend is true, complete the primitive that the parser is in the middle of.
 * If extend is false, it is assumed that the current char is a valid start of
 * a primitive.
 */
static int jsmn_parse_primitive(jsmn_parser *parser, const char *js,
                                const size_t len, jsmntok_t *tokens,
                                const size_t num_tokens, int extend) {
  jsmntok_t *token;
  int start;
#ifdef JSMN_STRICT
  int i;
  if (extend) {
    parser->pos = tokens[parser->toknext - 1].start;
  }
#endif
  start = parser->pos;

#ifdef JSMN_STRICT
  if (js[parser->pos] == 't') {
    for (i = 1;
         i < strlen(true_str);
         i++) {
      if (parser->pos + i >= len || js[parser->pos + i] == '\0') {
        return JSMN_ERROR_PART;
      } else if (js[parser->pos + i] != true_str[i]) {
        return JSMN_ERROR_INVAL;
      }
    }
    parser->pos += 4;
  } else if (js[parser->pos] == 'f') {
    for (i = 1;
         i < strlen(false_str);
         i++) {
      if (parser->pos + i >= len || js[parser->pos + i] == '\0') {
        return JSMN_ERROR_PART;
      } else if (js[parser->pos + i] != false_str[i]) {
        return JSMN_ERROR_INVAL;
      }
    }
    parser->pos += 5;
  } else if (js[parser->pos] == 'n') {
    for (i = 1;
         i < strlen(null_str);
         i++) {
      if (parser->pos + i >= len || js[parser->pos + i] == '\0') {
        return JSMN_ERROR_PART;
      } else if (js[parser->pos + i] != null_str[i]) {
        return JSMN_ERROR_INVAL;
      }
    }
    parser->pos += 4;
  } else {
    jsmn_numberstate numstate = JSMN_NUM_INT_START;
    for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
      switch (js[parser->pos]) {
      case '-':
        if (numstate == JSMN_NUM_INT_START ||
            numstate == JSMN_NUM_EXP_START) {
          numstate++;
        } else {
          parser->pos = start;
          return JSMN_ERROR_INVAL;
        }
        break;
      case '0':
        if (numstate == JSMN_NUM_INT_START ||
            numstate == JSMN_NUM_INT_HAVE_SIGN) {
          numstate = JSMN_NUM_INT_ZERO;
        } else if (numstate == JSMN_NUM_INT) {
          numstate = JSMN_NUM_INT;
        } else if (numstate == JSMN_NUM_FRAC ||
                   numstate == JSMN_NUM_FRAC_START) {
          numstate = JSMN_NUM_FRAC;
        } else if (numstate == JSMN_NUM_EXP ||
                   numstate == JSMN_NUM_EXP_START ||
                   numstate == JSMN_NUM_EXP_HAVE_SIGN) {
          numstate = JSMN_NUM_EXP;
        } else {
          parser->pos = start;
          return JSMN_ERROR_INVAL;
        }
        break;
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        if (numstate == JSMN_NUM_INT ||
            numstate == JSMN_NUM_INT_START ||
            numstate == JSMN_NUM_INT_HAVE_SIGN) {
          numstate = JSMN_NUM_INT;
        } else if (numstate == JSMN_NUM_FRAC ||
                   numstate == JSMN_NUM_FRAC_START) {
          numstate = JSMN_NUM_FRAC;
        } else if (numstate == JSMN_NUM_EXP ||
                   numstate == JSMN_NUM_EXP_START ||
                   numstate == JSMN_NUM_EXP_HAVE_SIGN) {
          numstate = JSMN_NUM_EXP;
        } else {
          parser->pos = start;
          return JSMN_ERROR_INVAL;
        }
        break;
      case '+':
        if (numstate == JSMN_NUM_EXP_START) {
          numstate++;
        } else {
          parser->pos = start;
          return JSMN_ERROR_INVAL;
        }
        break;
      case 'e':
      case 'E':
        if (numstate == JSMN_NUM_INT_ZERO ||
            numstate == JSMN_NUM_INT ||
            numstate == JSMN_NUM_FRAC) {
          numstate = JSMN_NUM_EXP_START;
        } else {
          parser->pos = start;
          return JSMN_ERROR_INVAL;
        }
        break;
      case '.':
        if (numstate == JSMN_NUM_INT_ZERO ||
            numstate == JSMN_NUM_INT) {
          numstate = JSMN_NUM_FRAC_START;
        } else {
          parser->pos = start;
          return JSMN_ERROR_INVAL;
        }
        break;
      default:
        goto check_number_complete;
      }
    }
check_number_complete:
    if (numstate != JSMN_NUM_INT &&
        numstate != JSMN_NUM_INT_ZERO &&
        numstate != JSMN_NUM_FRAC &&
        numstate != JSMN_NUM_EXP) {
      if (parser->pos >= len || js[parser->pos] == '\0') {
        parser->pos = start;
        return JSMN_ERROR_PART;
      } else {
        parser->pos = start;
        return JSMN_ERROR_INVAL;
      }
    }
  }

  /* Verify that what comes after the primitive is a non-primitive character */
  if (parser->pos < len) {
    switch (js[parser->pos]) {
    case '\t':
    case '\r':
    case '\n':
    case ' ':
    case ',':
    case ':':
    case '"':
    case '[':
    case ']':
    case '{':
    case '}':
    case '\0':
      break;
    default:
      parser->pos = start;
      return JSMN_ERROR_INVAL;
    }
  }
#else
  for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
    switch (js[parser->pos]) {
    case '\t':
    case '\r':
    case '\n':
    case ' ':
    case ',':
    case ':':
    case '"':
    case '[':
    case ']':
    case '{':
    case '}':
      goto found;
    default:       /* to quiet a warning from gcc*/
      break;
    }
    if (js[parser->pos] < 32 || js[parser->pos] >= 127) {
      parser->pos = start;
      return JSMN_ERROR_INVAL;
    }
  }
#endif

found:
  if (tokens == NULL) {
    parser->pos--;
    return 0;
  }
  if (extend) {
    tokens[parser->toknext - 1].end = parser->pos;
  } else {
    token = jsmn_alloc_token(parser, tokens, num_tokens);
    if (token == NULL) {
      parser->pos = start;
      return JSMN_ERROR_NOMEM;
    }
    jsmn_fill_token(token, JSMN_PRIMITIVE, start, parser->pos);
    parser->pos--;
  }
  return 0;
}

/**
 * Fills next token with JSON string.
 */
static int jsmn_parse_string(jsmn_parser *parser, const char *js,
                             const size_t len, jsmntok_t *tokens,
                             const size_t num_tokens) {
  jsmntok_t *token;
  unsigned int start = parser->pos;

  /* Skip starting quote */
  parser->pos++;

  for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
    char c = js[parser->pos];

    /* Quote: end of string */
    if (c == '\"') {
      if (tokens == NULL) {
        return 0;
      }
      token = jsmn_alloc_token(parser, tokens, num_tokens);
      if (token == NULL) {
        parser->pos = start;
        return JSMN_ERROR_NOMEM;
      }
      jsmn_fill_token(token, JSMN_STRING, start + 1, parser->pos);
      return 0;
    }

    /* Backslash: Quoted symbol expected */
    if (c == '\\' && parser->pos + 1 < len) {
      int i;
      parser->pos++;
#ifdef JSMN_STRICT
      switch (js[parser->pos]) {
      /* Allowed escaped symbols */
      case '\"':
      case '/':
      case '\\':
      case 'b':
      case 'f':
      case 'r':
      case 'n':
      case 't':
        break;
      /* Allows escaped symbol \uXXXX */
      case 'u':
        parser->pos++;
        for (i = 0; i < 4 && parser->pos < len && js[parser->pos] != '\0';
             i++) {
          /* If it isn't a hex character we have an error */
          if (!((js[parser->pos] >= 48 && js[parser->pos] <= 57) ||   /* 0-9 */
                (js[parser->pos] >= 65 && js[parser->pos] <= 70) ||   /* A-F */
                (js[parser->pos] >= 97 && js[parser->pos] <= 102))) { /* a-f */
            parser->pos = start;
            return JSMN_ERROR_INVAL;
          }
          parser->pos++;
        }
        parser->pos--;
        break;
      /* Unexpected symbol */
      default:
        parser->pos = start;
        return JSMN_ERROR_INVAL;
      }
#endif
    }

#ifdef JSMN_STRICT
    /* No control characters allowed in strict mode */
    if (c >= 0 && c < 32) {
      parser->pos = start;
      return JSMN_ERROR_INVAL;
    }
#endif
  }
  parser->pos = start;
  return JSMN_ERROR_PART;
}

/**
 * If in the middle of a primitive, add the rest of it to the current primitive
 * token
 */
static int jsmn_finish_primitive(jsmn_parser *parser, const char *js,
                                   const size_t len, jsmntok_t *tokens,
                                   const size_t num_tokens) {
  unsigned int pos = parser->pos;
  if (parser->toknext > 0 && parser->pos < len) {
    if (js[pos] > 32 && js[pos] < 127 && js[pos] != ',' && js[pos] != ':' &&
        js[pos] != '{' && js[pos] != '}' && js[pos] != '[' && js[pos] != ']' &&
        js[pos] != '"' && js[pos - 1] > 32 && js[pos - 1] < 127 &&
        js[pos - 1] != ',' && js[pos - 1] != ':' && js[pos - 1] != '{' &&
        js[pos - 1] != '}' && js[pos - 1] != '[' && js[pos - 1] != ']' &&
        js[pos - 1] != '"') {
      return jsmn_parse_primitive(parser, js, len, tokens, num_tokens, 1);
    }
  }
  return 0;
}

/**
 * Parse JSON string and fill tokens.
 */
JSMN_API int jsmn_parse(jsmn_parser *parser, const char *js, const size_t len,
                        jsmntok_t *tokens, const unsigned int num_tokens) {
  int r;
#ifndef JSMN_PARENT_LINKS
  int i;
#endif
  jsmntok_t *token;
  int depth = 0; /* Obj/array nesting depth. Used only when tokens == NULL */
  int count = parser->toknext;

  r = jsmn_finish_primitive(parser, js, len, tokens, num_tokens);
  if (r < 0) {
    return r;
  }

  for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
    char c;

    c = js[parser->pos];
    switch (c) {
    case '{':
    case '[':
      count++;
      if (tokens == NULL) {
        depth++;
        break;
      }
      if (parser->state & JSMN_KEY) {
        return JSMN_ERROR_INVAL;
      }
      token = jsmn_alloc_token(parser, tokens, num_tokens);
      if (token == NULL) {
        return JSMN_ERROR_NOMEM;
      }
      if (parser->toksuper != parser->tokbefore) {
        tokens[parser->toksuper].size++;
#ifdef JSMN_PARENT_LINKS
        token->parent = parser->toksuper;
#endif
      }
      parser->toksuper = parser->toknext - 1;
      token->start = parser->pos;
      if (c == '{') {
        token->type = JSMN_OBJECT;
        parser->state = JSMN_STATE_OBJ_NEW;
      } else {
        token->type = JSMN_ARRAY;
        parser->state = JSMN_STATE_ARRAY_NEW;
      }
      break;
    case '}':
      if (tokens == NULL) {
        depth--;
        break;
      } else if (!(parser->state & JSMN_IN_OBJECT) ||
                 !(parser->state & JSMN_CAN_CLOSE)) {
        return JSMN_ERROR_INVAL;
      }
      if (parser->state & JSMN_VALUE) {
#ifdef JSMN_PARENT_LINKS
        parser->toksuper = tokens[parser->toksuper].parent;
#else
        for (i = parser->toksuper - 1; i != parser->tokbefore; i--) {
          if (tokens[i].end == 0) {
            break;
          }
        }
        parser->toksuper = i;
#endif
      }
      goto container_close;
    case ']':
      if (tokens == NULL) {
        depth--;
        break;
      } else if (!(parser->state & JSMN_IN_ARRAY) ||
                 !(parser->state & JSMN_CAN_CLOSE)) {
        return JSMN_ERROR_INVAL;
      }
container_close:
      token = &tokens[parser->toksuper];
      token->end = parser->pos + 1;
#ifdef JSMN_PARENT_LINKS
      parser->toksuper = token->parent;
#else
      if (parser->toksuper - 1 == parser->tokbefore || token[-1].size > 0) {
        parser->toksuper--;
      } else {
        for (i = parser->toksuper - 2; i != parser->tokbefore; i--) {
          if (tokens[i].end == 0) {
            break;
          }
        }
        parser->toksuper = i;
      }
#endif
      if (parser->toksuper == parser->tokbefore) {
        parser->tokbefore = parser->toknext - 1;
        parser->toksuper = parser->toknext - 1;
        parser->state = JSMN_STATE_ROOT;
      } else {
        parser->state = (tokens[parser->toksuper].type == JSMN_ARRAY) ?
                        JSMN_STATE_ARRAY_COMMA : JSMN_STATE_OBJ_COMMA;
      }
      break;
    case '\"':
      count++;
      r = jsmn_parse_string(parser, js, len, tokens, num_tokens);
      if (r < 0) {
        return r;
      }
      if (tokens == NULL) {
        break;
      }
      if (parser->toksuper == parser->tokbefore) {
        parser->tokbefore = parser->toknext - 1;
        parser->toksuper = parser->toknext - 1;
        parser->state = JSMN_STATE_ROOT;
        break;
      } else if (parser->state & JSMN_DELIMITER) {
        return JSMN_ERROR_INVAL;
      }
      tokens[parser->toksuper].size++;
#ifdef JSMN_PARENT_LINKS
      tokens[parser->toknext - 1].parent = parser->toksuper;
#endif
      if (parser->state & JSMN_KEY) {
        parser->state = JSMN_STATE_OBJ_COLON;
      } else {
        parser->state |= JSMN_DELIMITER | JSMN_CAN_CLOSE;
      }
      break;
    case '\t':
    case '\r':
    case '\n':
    case ' ':
      break;
    case ':':
      if (tokens == NULL) {
        break;
      } else if (parser->state != JSMN_STATE_OBJ_COLON) {
        return JSMN_ERROR_INVAL;
      }
      parser->toksuper = parser->toknext - 1;
      parser->state = JSMN_STATE_OBJ_VAL;
      break;
    case ',':
      if (tokens == NULL) {
        break;
      } else if (parser->state == JSMN_STATE_OBJ_COMMA) {
#ifdef JSMN_PARENT_LINKS
        parser->toksuper = tokens[parser->toksuper].parent;
#else
        for (i = parser->toksuper - 1; tokens[i].end != 0; i--);
        parser->toksuper = i;
#endif
        parser->state = JSMN_STATE_OBJ_KEY;
      } else if (parser->state == JSMN_STATE_ARRAY_COMMA) {
        parser->state = JSMN_STATE_ARRAY_ITEM;
      } else {
        return JSMN_ERROR_INVAL;
      }
      break;
#ifdef JSMN_STRICT
    /* In strict mode primitives are: numbers and booleans */
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case 't':
    case 'f':
    case 'n':
#else
    default:
#endif
      count++;
      r = jsmn_parse_primitive(parser, js, len, tokens, num_tokens, 0);
      if (r < 0) {
        return r;
      }
      if (tokens == NULL) {
        break;
      }
      if (parser->toksuper == parser->tokbefore) {
        parser->tokbefore = parser->toknext - 1;
        parser->toksuper = parser->toknext - 1;
        parser->state = JSMN_STATE_ROOT;
        break;
#ifdef JSMN_STRICT
      } else if (parser->state & (JSMN_DELIMITER | JSMN_KEY)) {
#else
      } else if (parser->state & JSMN_DELIMITER) {
#endif
        return JSMN_ERROR_INVAL;
      }
      tokens[parser->toksuper].size++;
#ifdef JSMN_PARENT_LINKS
      tokens[parser->toknext - 1].parent = parser->toksuper;
#endif
#ifdef JSMN_STRICT
      parser->state |= JSMN_DELIMITER | JSMN_CAN_CLOSE;
#else
      if (parser->state & JSMN_KEY) {
        parser->state = JSMN_STATE_OBJ_COLON;
      } else {
        parser->state |= JSMN_DELIMITER | JSMN_CAN_CLOSE;
      }
#endif
      break;
#ifdef JSMN_STRICT
    /* Unexpected char in strict mode */
    default:
      return JSMN_ERROR_INVAL;
#endif
    }
  }

  if (depth == 0 && parser->state == JSMN_STATE_ROOT) {
    return count;
  } else {
    return JSMN_ERROR_PART;
  }
}

/**
 * Creates a new parser based over a given buffer with an array of tokens
 * available.
 */
JSMN_API void jsmn_init(jsmn_parser *parser) {
  parser->pos = 0;
  parser->toknext = 0;
  parser->toksuper = -1;
  parser->state = JSMN_STATE_ROOT;
  parser->tokbefore = -1;
}

#endif /* JSMN_HEADER */

#ifdef __cplusplus
}
#endif

#endif /* JSMN_H */
