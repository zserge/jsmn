/*
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

#ifndef JSMN_API
#  ifdef JSMN_STATIC
#    define JSMN_API static
#  else
#    define JSMN_API extern
#  endif
#endif

#ifdef JSMN_SHORT_TOKENS
typedef unsigned short jsmnint_t;
#  define packed __attribute__((packed))
#else
typedef unsigned int jsmnint_t;
#  define packed
#endif
#define JSMN_NEG ((jsmnint_t)-1)

/**
 * JSON type identifier. Basic types are:
 */
typedef enum packed {
  JSMN_UNDEFINED    = 0x0000,
  JSMN_OBJECT       = 0x0001,   //!< Object
  JSMN_ARRAY        = 0x0002,   //!< Array
  JSMN_STRING       = 0x0004,   //!< String
  JSMN_PRIMITIVE    = 0x0008,   //!< Other primitive: number, boolean (true/false) or null

  JSMN_KEY          = 0x0010,   //!< is a key
  JSMN_VALUE        = 0x0020,   //!< is a value

  /* Complex elements */
  JSMN_CONTAINER    = JSMN_OBJECT | JSMN_ARRAY,
  JSMN_ANY_TYPE     = JSMN_OBJECT | JSMN_ARRAY | JSMN_STRING | JSMN_PRIMITIVE,

  JSMN_OBJ_VAL      = JSMN_OBJECT | JSMN_VALUE,
  JSMN_ARR_VAL      = JSMN_ARRAY  | JSMN_VALUE,
  JSMN_STR_KEY      = JSMN_STRING | JSMN_KEY,
  JSMN_STR_VAL      = JSMN_STRING | JSMN_VALUE,
  JSMN_PRI_VAL      = JSMN_PRIMITIVE | JSMN_VALUE,
#ifdef JSMN_PERMISSIVE
  JSMN_OBJ_KEY      = JSMN_OBJECT | JSMN_KEY,
  JSMN_ARR_KEY      = JSMN_ARRAY  | JSMN_KEY,
  JSMN_PRI_KEY      = JSMN_PRIMITIVE | JSMN_KEY,
#endif

  /* Primitive extension */
  JSMN_PRI_LITERAL  = 0x0040,   //!< true, false, null
  JSMN_PRI_INTEGER  = 0x0080,   //!< 0, 1 - 9
  JSMN_PRI_SIGN     = 0x0100,   //!< minus sign, '-' or plus sign, '+'
  JSMN_PRI_DECIMAL  = 0x0200,   //!< deminal point '.'
  JSMN_PRI_EXPONENT = 0x0400,   //!< exponent, 'e' or 'E'

  JSMN_PRI_MINUS    = JSMN_PRI_SIGN,

  /* Parsing validation, expectations, and state information */
  JSMN_CLOSE        = 0x0800,   //!< Close OBJECT '}' or ARRAY ']'
  JSMN_DELIMITER    = 0x1000,   //!< Colon ':' after KEY, Comma ',' after VALUE
  JSMN_PREV_KEY     = 0x2000,   //!< Previous token is an KEY
  JSMN_PREV_VAL     = 0x4000,   //!< Previous token is a VALUE
  JSMN_INSD_OBJ     = 0x8000,   //!< Inside an OBJECT

  JSMN_COLON        = JSMN_DELIMITER | JSMN_PREV_KEY,
  JSMN_COMMA        = JSMN_DELIMITER | JSMN_PREV_VAL,

  /* Parsing rules */
  JSMN_ROOT_INIT    = JSMN_ANY_TYPE | JSMN_VALUE,
#ifndef JSMN_PERMISSIVE
  JSMN_ROOT         = JSMN_ANY_TYPE | JSMN_VALUE,
  JSMN_OPEN_OBJECT  = JSMN_STRING   | JSMN_KEY   | JSMN_CLOSE | JSMN_INSD_OBJ,
  JSMN_AFTR_OBJ_KEY =                 JSMN_VALUE |              JSMN_INSD_OBJ | JSMN_COLON,
  JSMN_AFTR_OBJ_VAL =                 JSMN_KEY   | JSMN_CLOSE | JSMN_INSD_OBJ |              JSMN_COMMA,
  JSMN_OPEN_ARRAY   = JSMN_ANY_TYPE | JSMN_VALUE | JSMN_CLOSE,
  JSMN_AFTR_ARR_VAL =                 JSMN_VALUE | JSMN_CLOSE |                              JSMN_COMMA,
  JSMN_AFTR_CLOSE   =                              JSMN_CLOSE |                              JSMN_COMMA,
  JSMN_AFTR_COLON   = JSMN_ANY_TYPE | JSMN_VALUE |              JSMN_INSD_OBJ |                           JSMN_PREV_KEY,
  JSMN_AFTR_COMMA_O = JSMN_STRING   | JSMN_KEY   |              JSMN_INSD_OBJ |                           JSMN_PREV_VAL,
  JSMN_AFTR_COMMA_A = JSMN_ANY_TYPE | JSMN_VALUE |                                                        JSMN_PREV_VAL,
#else
  JSMN_ROOT         = JSMN_ANY_TYPE |                                           JSMN_COLON | JSMN_COMMA,
  JSMN_ROOT_AFTR_O  = JSMN_ANY_TYPE |                                                        JSMN_COMMA,
  JSMN_OPEN_OBJECT  = JSMN_ANY_TYPE | JSMN_KEY   | JSMN_CLOSE | JSMN_INSD_OBJ,
  JSMN_AFTR_OBJ_KEY =                 JSMN_VALUE |              JSMN_INSD_OBJ | JSMN_COLON,
  JSMN_AFTR_OBJ_VAL = JSMN_ANY_TYPE |              JSMN_CLOSE | JSMN_INSD_OBJ |              JSMN_COMMA,
  JSMN_OPEN_ARRAY   = JSMN_ANY_TYPE | JSMN_VALUE | JSMN_CLOSE,
  JSMN_AFTR_ARR_VAL = JSMN_ANY_TYPE |              JSMN_CLOSE |                 JSMN_COLON | JSMN_COMMA,
  JSMN_AFTR_CLOSE   = JSMN_ANY_TYPE |              JSMN_CLOSE |                              JSMN_COMMA,
  JSMN_AFTR_COLON   = JSMN_ANY_TYPE | JSMN_VALUE |              JSMN_INSD_OBJ |                           JSMN_PREV_KEY,
  JSMN_AFTR_COLON_R = JSMN_ANY_TYPE | JSMN_VALUE |                                                        JSMN_PREV_KEY,
  JSMN_AFTR_COMMA_O = JSMN_ANY_TYPE | JSMN_KEY   |              JSMN_INSD_OBJ |                           JSMN_PREV_VAL,
  JSMN_AFTR_COMMA_A = JSMN_ANY_TYPE | JSMN_VALUE |                                                        JSMN_PREV_VAL,
  JSMN_AFTR_COMMA_R = JSMN_ANY_TYPE |                                                                     JSMN_PREV_VAL,
#endif
} jsmntype_t;

enum jsmnerr {
  JSMN_SUCCESS     =  0,
  JSMN_ERROR_NOMEM = -1,        //!< Not enough tokens were provided
  JSMN_ERROR_INVAL = -2,        //!< Invalid character inside JSON string
  JSMN_ERROR_PART  = -3,        //!< The string is not a full JSON packet, more bytes expected
  JSMN_ERROR_LEN   = -4,        //!< Input data too long
};

/**
 * JSON token description.
 */
typedef struct jsmntok_t {
  jsmntype_t type;              //!< type (object, array, string etc.)
  jsmnint_t start;              //!< start position in JSON data string
  jsmnint_t end;                //!< end position in JSON data string
  jsmnint_t size;               //!< number of children
#ifdef JSMN_PARENT_LINKS
  jsmnint_t parent;             //!< parent id
#endif
#ifdef JSMN_NEXT_SIBLING
  jsmnint_t next_sibling;       //!< next sibling id
#endif
} jsmntok_t;

/**
 * JSON parser
 *
 * Contains an array of token blocks available. Also stores
 * the string being parsed now and current position in that string.
 */
typedef struct jsmn_parser {
  jsmnint_t pos;            //!< offset in the JSON string
  jsmnint_t toknext;        //!< next token to allocate
                            //!< when tokens == NULL, keeps track of container types to a depth of (sizeof(jsmnint_t) * 8)
  jsmnint_t toksuper;       //!< superior token node, e.g. parent object or array
                            //!< when tokens == NULL, toksuper represents container depth
  jsmntype_t expected;      //!< Expected jsmn type(s)
} jsmn_parser;

/**
 * @brief Create JSON parser over an array of tokens
 *
 * @param[out] parser jsmn parser
 */
JSMN_API
void jsmn_init(jsmn_parser *parser);

/**
 * @brief Run JSON parser
 *
 * It parses a JSON data string into and array of tokens, each
 * describing a single JSON object.
 *
 * @param[in,out] parser jsmn parser
 * @param[in] js JSON data string
 * @param[in] len JSON data string length
 * @param[in,out] tokens pointer to memory allocated for tokens or NULL
 * @param[in] num_tokens number of tokens allocated
 * @return jsmnint_t number of tokens found or ERRNO
 */
JSMN_API
jsmnint_t jsmn_parse(jsmn_parser *parser, const char *js,
                     const size_t len, jsmntok_t *tokens,
                     const size_t num_tokens);

#ifndef JSMN_HEADER
/**
 * Allocates a fresh unused token from the token pool.
 */
static
jsmntok_t *jsmn_alloc_token(jsmn_parser *parser, jsmntok_t *tokens,
                            const size_t num_tokens) {
  if (parser->toknext >= num_tokens) {
    return NULL;
  }

  jsmntok_t *tok;
  tok = &tokens[parser->toknext++];
  tok->start = tok->end = JSMN_NEG;
  tok->size = 0;
#ifdef JSMN_PARENT_LINKS
  tok->parent = JSMN_NEG;
#endif
#ifdef JSMN_NEXT_SIBLING
  tok->next_sibling = JSMN_NEG;
#endif
  return tok;
}

/**
 * Fills token type and boundaries.
 */
static
void jsmn_fill_token(jsmntok_t *token, const jsmntype_t type,
                     const jsmnint_t start, const jsmnint_t end) {
  token->type = type;
  token->start = start;
  token->end = end;
  token->size = 0;
}

#ifdef JSMN_NEXT_SIBLING
/**
 * Set previous child's next_sibling to current token
 */
static
void jsmn_next_sibling(jsmn_parser *parser, jsmntok_t *tokens) {
  jsmnint_t sibling;

  /* Start with parent's first child */
  if (parser->toksuper != JSMN_NEG) {
    sibling = parser->toksuper + 1;
  } else {
    sibling = 0;
  }

  /* If the first child is the current token */
  if (sibling == parser->toknext - 1)
    return;

  /* Loop until we find previous sibling */
  while (tokens[sibling].next_sibling != JSMN_NEG)
    sibling = tokens[sibling].next_sibling;

  /* Set previous sibling's next_sibling to current token */
  tokens[sibling].next_sibling = parser->toknext - 1;
}
#endif

/**
 * Fills next available token with JSON primitive.
 */
static
jsmnint_t jsmn_parse_primitive(jsmn_parser *parser, const char *js,
                               const size_t len, jsmntok_t *tokens,
                               const size_t num_tokens) {
  /* If a PRIMITIVE wasn't expected */
  if (!(parser->expected & JSMN_PRIMITIVE)) {
    return JSMN_ERROR_INVAL;
  }

  jsmnint_t start;
  jsmntype_t type;
  jsmntype_t expected;

  start = parser->pos;
  type = JSMN_PRIMITIVE;

#ifndef JSMN_PERMISSIVE
  if (js[parser->pos] == 't' || js[parser->pos] == 'f' || js[parser->pos] == 'n') {
    char *literal = NULL;
    jsmnint_t size = 0;
    if (js[parser->pos] == 't') {
      literal = "true";
      size = 4;
    } else if (js[parser->pos] == 'f') {
      literal = "false";
      size = 5;
    } else if (js[parser->pos] == 'n') {
      literal = "null";
      size = 4;
    }
    jsmnint_t i;
    for (i = 1, parser->pos++; i < size; i++, parser->pos++) {
      if (parser->pos >= len || js[parser->pos] == '\0') {
        parser->pos = start;
        return JSMN_ERROR_PART;
      } else if (js[parser->pos] != literal[i]) {
        parser->pos = start;
        return JSMN_ERROR_INVAL;
      }
    }
    type |= JSMN_PRI_LITERAL;
  } else {
    expected = JSMN_PRIMITIVE | JSMN_PRI_MINUS | JSMN_PRI_INTEGER;
    for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
      switch (js[parser->pos]) {
      case '0':
        if (!(expected & JSMN_PRI_INTEGER)) {
          parser->pos = start;
          return JSMN_ERROR_INVAL;
        }
        if (type & JSMN_PRI_EXPONENT) {
          expected = JSMN_PRIMITIVE | JSMN_PRI_INTEGER;
        } else if (type & JSMN_PRI_DECIMAL) {
          expected = JSMN_PRIMITIVE | JSMN_PRI_INTEGER | JSMN_PRI_EXPONENT;
        } else if (start == parser->pos ||
            (start + 1 == parser->pos && (type & JSMN_PRI_MINUS))) {
          expected = JSMN_PRIMITIVE | JSMN_PRI_DECIMAL | JSMN_PRI_EXPONENT;
        } else {
          expected = JSMN_PRIMITIVE | JSMN_PRI_INTEGER | JSMN_PRI_DECIMAL | JSMN_PRI_EXPONENT;
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
        if (!(expected & JSMN_PRI_INTEGER)) {
          parser->pos = start;
          return JSMN_ERROR_INVAL;
        }
        if (type & JSMN_PRI_EXPONENT) {
          expected = JSMN_PRIMITIVE | JSMN_PRI_INTEGER;
        } else if (type & JSMN_PRI_DECIMAL) {
          expected = JSMN_PRIMITIVE | JSMN_PRI_INTEGER | JSMN_PRI_EXPONENT;
        } else {
          expected = JSMN_PRIMITIVE | JSMN_PRI_INTEGER | JSMN_PRI_DECIMAL | JSMN_PRI_EXPONENT;
        }
        break;
      case '-':
        if (!(expected & JSMN_PRI_MINUS)) {
          parser->pos = start;
          return JSMN_ERROR_INVAL;
        }
        expected = JSMN_PRIMITIVE | JSMN_PRI_INTEGER;
        if (start == parser->pos) {
          type |= JSMN_PRI_MINUS;
        }
        break;
      case '+':
        if (!(expected & JSMN_PRI_SIGN) && start + 2 < parser->pos) {
          parser->pos = start;
          return JSMN_ERROR_INVAL;
        }
        expected = JSMN_PRIMITIVE | JSMN_PRI_INTEGER;
        break;
      case '.':
        if (!(expected & JSMN_PRI_DECIMAL)) {
          parser->pos = start;
          return JSMN_ERROR_INVAL;
        }
        type |= JSMN_PRI_DECIMAL;
        expected = JSMN_PRIMITIVE | JSMN_PRI_INTEGER;
        break;
      case 'e':
      case 'E':
        if (!(expected & JSMN_PRI_EXPONENT)) {
          parser->pos = start;
          return JSMN_ERROR_INVAL;
        }
        type |= JSMN_PRI_EXPONENT;
        expected = JSMN_PRIMITIVE | JSMN_PRI_SIGN | JSMN_PRI_INTEGER;
        break;
      default:
        if (parser->pos >= len || js[parser->pos] == '\0') {
          parser->pos = start;
          return JSMN_ERROR_PART;
        } else if (start + 1 == parser->pos && type & JSMN_PRI_MINUS) {
          parser->pos = start;
          return JSMN_ERROR_INVAL;
        }
        goto check_primitive_border;
      }
    }
  }
check_primitive_border:
  switch (js[parser->pos]) {
  case ' ':
  case '\t':
  case '\n':
  case '\r':
  case ',':
  case '}':
  case ']':
    goto found;
  case '"':
  case ':':
  case '{':
  case '[':
    parser->pos = start;
    return JSMN_ERROR_INVAL;
  case '\0':
    if (parser->toksuper != JSMN_NEG) {
      parser->pos = start;
      return JSMN_ERROR_PART;
    }
    goto found;
  default:
    parser->pos = start;
    return JSMN_ERROR_PART;
  }
#else
  for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
    switch (js[parser->pos]) {
    case ' ':
    case '\t':
    case '\n':
    case '\r':
    case ',':
    case '}':
    case ']':
    case ':':
      goto found;
    default:                /* to quiet a warning from gcc */
      break;
    }
    if (js[parser->pos] < 32 || js[parser->pos] >= 127) {
      parser->pos = start;
      return JSMN_ERROR_INVAL;
    }
  }
#endif

found:
  expected = parser->expected;
  if (parser->toksuper != JSMN_NEG) {
    /* OBJECT KEY, strict query */
    if ((parser->expected & (JSMN_KEY | JSMN_INSD_OBJ)) == (JSMN_KEY | JSMN_INSD_OBJ)) {
      parser->expected = JSMN_AFTR_OBJ_KEY;
      type |= JSMN_KEY   | JSMN_INSD_OBJ;
    /* OBJECT VALUE, VALUE is implicit */
    } else if (parser->expected & JSMN_INSD_OBJ) {
      parser->expected = JSMN_AFTR_OBJ_VAL;
      type |= JSMN_VALUE | JSMN_INSD_OBJ;
#ifdef JSMN_PERMISSIVE
    /* OBJECT VALUE at the ROOT level */
    } else if (parser->expected == JSMN_AFTR_COLON_R) {
      parser->expected = JSMN_ROOT_AFTR_O;
      type |= JSMN_VALUE;
#endif
    /* ARRAY VALUE, VALUE is implicit */
    } else {
      parser->expected = JSMN_AFTR_ARR_VAL;
      type |= JSMN_VALUE;
    }
  } else {
    parser->expected = JSMN_ROOT;
    type |= JSMN_VALUE;
  }

  if (tokens == NULL) {
    parser->pos--;
    return JSMN_SUCCESS;
  }

  jsmntok_t *token;
  token = jsmn_alloc_token(parser, tokens, num_tokens);
  if (token == NULL) {
    parser->expected = expected;
    parser->pos = start;
    return JSMN_ERROR_NOMEM;
  }
  jsmn_fill_token(token, type, start, parser->pos);
#ifdef JSMN_PARENT_LINKS
  token->parent = parser->toksuper;
#endif
#ifdef JSMN_NEXT_SIBLING
  jsmn_next_sibling(parser, tokens);
#endif

  if (parser->toksuper != JSMN_NEG) {
    tokens[parser->toksuper].size++;

    if (!(tokens[parser->toksuper].type & JSMN_CONTAINER)) {
#ifdef JSMN_PARENT_LINKS
      parser->toksuper = tokens[parser->toksuper].parent;
#else
      jsmnint_t i;
      for (i = parser->toksuper; i != JSMN_NEG; i--) {
        if (tokens[i].type & JSMN_CONTAINER && tokens[i].end == JSMN_NEG) {
          parser->toksuper = i;
          break;
        }
      }
#  ifdef JSMN_PERMISSIVE
      if (i == JSMN_NEG) {
        parser->toksuper = i;
      }
#  endif
#endif
    }
  }
  parser->pos--;

  return JSMN_SUCCESS;
}

/**
 * Fills next token with JSON string.
 */
static
jsmnint_t jsmn_parse_string(jsmn_parser *parser, const char *js,
                            const size_t len, jsmntok_t *tokens,
                            const size_t num_tokens) {
  /* If a STRING wasn't expected */
  if (!(parser->expected & JSMN_STRING)) {
    return JSMN_ERROR_INVAL;
  }

  if (len >= JSMN_NEG) {
    return JSMN_ERROR_LEN;
  }

  jsmnint_t start;
  start = parser->pos;

  /* Skip starting quote */
  parser->pos++;

  char c;
  for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
    c = js[parser->pos];

    /* Quote: end of string */
    if (c == '\"') {
      jsmntype_t expected = parser->expected;
      jsmntype_t type;
      if (parser->toksuper != JSMN_NEG) {
        /* OBJECT KEY, strict query */
        if ((parser->expected & (JSMN_INSD_OBJ | JSMN_KEY)) == (JSMN_INSD_OBJ | JSMN_KEY)) {
          parser->expected = JSMN_AFTR_OBJ_KEY;
          type = JSMN_STRING | JSMN_KEY   | JSMN_INSD_OBJ;
        /* OBJECT VALUE, VALUE is implicit */
        } else if (parser->expected & JSMN_INSD_OBJ) {
          parser->expected = JSMN_AFTR_OBJ_VAL;
          type = JSMN_STRING | JSMN_VALUE | JSMN_INSD_OBJ;
#ifdef JSMN_PERMISSIVE
        /* OBJECT VALUE at the ROOT level */
        } else if (parser->expected == JSMN_AFTR_COLON_R) {
          parser->expected = JSMN_ROOT_AFTR_O;
          type = JSMN_STRING | JSMN_VALUE;
#endif
        /* ARRAY VALUE, VALUE is implicit */
        } else {
          parser->expected = JSMN_AFTR_ARR_VAL;
          type = JSMN_STRING | JSMN_VALUE;
        }
      } else {
        parser->expected = JSMN_ROOT;
        type = JSMN_STRING | JSMN_VALUE;
      }

      if (tokens == NULL) {
        return JSMN_SUCCESS;
      }

      jsmntok_t *token;
      token = jsmn_alloc_token(parser, tokens, num_tokens);
      if (token == NULL) {
        parser->expected = expected;
        parser->pos = start;
        return JSMN_ERROR_NOMEM;
      }
      jsmn_fill_token(token, type, start + 1, parser->pos);
#ifdef JSMN_PARENT_LINKS
      token->parent = parser->toksuper;
#endif
#ifdef JSMN_NEXT_SIBLING
      jsmn_next_sibling(parser, tokens);
#endif

      if (parser->toksuper != JSMN_NEG) {
        tokens[parser->toksuper].size++;

        if (!(tokens[parser->toksuper].type & JSMN_CONTAINER)) {
#ifdef JSMN_PARENT_LINKS
          parser->toksuper = tokens[parser->toksuper].parent;
#else
          jsmnint_t i;
          for (i = parser->toksuper; i != JSMN_NEG; i--) {
            if (tokens[i].type & JSMN_CONTAINER && tokens[i].end == JSMN_NEG) {
              parser->toksuper = i;
              break;
            }
          }
#  ifdef JSMN_PERMISSIVE
          if (i == JSMN_NEG) {
            parser->toksuper = i;
          }
#  endif
#endif
        }
      }

      return JSMN_SUCCESS;
    }

    /* Backslash: Quoted symbol expected */
    if (c == '\\' && parser->pos + 1 < len) {
      parser->pos++;
      switch (js[parser->pos]) {
      /* Allowed escaped symbols */
      case '\"':
      case '\\':
      case '/':
      case 'b':
      case 'f':
      case 'n':
      case 'r':
      case 't':
        break;
      /* Allows escaped symbol \uXXXX */
      case 'u':
        parser->pos++;
        jsmnint_t i;
        for (i = 0; i < 4 && parser->pos < len && js[parser->pos] != '\0'; i++) {
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
    }
  }
  parser->pos = start;
  return JSMN_ERROR_PART;
}

static
jsmnint_t jsmn_parse_container_open(jsmn_parser *parser, const char c,
                                    jsmntok_t *tokens, const size_t num_tokens) {
  /* If an OBJECT or ARRAY wasn't expected */
  if (!(parser->expected & JSMN_CONTAINER)) {
    return JSMN_ERROR_INVAL;
  }

  jsmntype_t type;
  if (c == '{') {
    parser->expected = JSMN_OPEN_OBJECT;
    type = JSMN_OBJECT | JSMN_VALUE;
  } else {
    parser->expected = JSMN_OPEN_ARRAY;
    type = JSMN_ARRAY  | JSMN_VALUE;
  }

  if (tokens == NULL) {
    parser->toksuper++;
    if (parser->toksuper < (sizeof(jsmnint_t) * 8) &&
        parser->expected & JSMN_INSD_OBJ) {
      parser->toknext |= (1 << parser->toksuper);
    }
    return JSMN_SUCCESS;
  }

  if (parser->toksuper != JSMN_NEG &&
      tokens[parser->toksuper].type & JSMN_INSD_OBJ) {
    type |= JSMN_INSD_OBJ;
  }

  jsmntok_t *token;
  token = jsmn_alloc_token(parser, tokens, num_tokens);
  if (token == NULL) {
    return JSMN_ERROR_NOMEM;
  }
  jsmn_fill_token(token, type, parser->pos, JSMN_NEG);
#ifdef JSMN_PARENT_LINKS
  token->parent = parser->toksuper;
#endif
#ifdef JSMN_NEXT_SIBLING
  jsmn_next_sibling(parser, tokens);
#endif

  if (parser->toksuper != JSMN_NEG) {
    tokens[parser->toksuper].size++;
  }
  parser->toksuper = parser->toknext - 1;

  return JSMN_SUCCESS;
}

static
jsmnint_t jsmn_parse_container_close(jsmn_parser *parser, const char c,
                                     jsmntok_t *tokens) {
  /* If an OBJECT or ARRAY close wasn't expected */
  if (!(parser->expected & JSMN_CLOSE)) {
    return JSMN_ERROR_INVAL;
  }

  if (tokens == NULL) {
    if (parser->toksuper < (sizeof(jsmnint_t) * 8)) {
      jsmntype_t type;
      type = (c == '}' ? JSMN_OBJECT : JSMN_ARRAY);
      if ((((parser->toknext & 1 << parser->toksuper) == 1) && !(type & JSMN_OBJECT)) ||
          (((parser->toknext & 1 << parser->toksuper) == 0) && !(type & JSMN_ARRAY))) {
        return JSMN_ERROR_INVAL;
      }
      parser->toknext &= ~(1 << parser->toksuper);
    }
    parser->toksuper--;
  } else {
    jsmntype_t type;
    jsmntok_t *token;

    type = (c == '}' ? JSMN_OBJECT : JSMN_ARRAY);
    token = &tokens[parser->toksuper];
    if (!(token->type & type) || token->end != JSMN_NEG) {
      return JSMN_ERROR_INVAL;
    }
    token->end = parser->pos + 1;
#ifdef JSMN_PARENT_LINKS
    if (token->type & JSMN_INSD_OBJ) {
      parser->toksuper = tokens[token->parent].parent;
    } else {
      parser->toksuper = token->parent;
    }
#else
    jsmnint_t i;
    for (i = parser->toksuper - 1; i != JSMN_NEG; i--) {
      if (tokens[i].type & JSMN_CONTAINER && tokens[i].end == JSMN_NEG) {
        parser->toksuper = i;
        break;
      }
    }
    if (i == JSMN_NEG) {
      parser->toksuper = i;
    }
#endif
  }

  if (parser->toksuper != JSMN_NEG) {
    parser->expected = JSMN_AFTR_CLOSE;
  } else {
    parser->expected = JSMN_ROOT;
  }

  return JSMN_SUCCESS;
}

static
jsmnint_t jsmn_parse_colon(jsmn_parser *parser, jsmntok_t *tokens) {
  /* If a COLON wasn't expected, strict check because it is a complex enum */
  if (!((parser->expected & JSMN_COLON) == JSMN_COLON)) {
    return JSMN_ERROR_INVAL;
  }

  if (parser->toksuper != JSMN_NEG) {
    parser->expected = JSMN_AFTR_COLON;
#ifdef JSMN_PERMISSIVE
  } else {
    parser->expected = JSMN_AFTR_COLON_R;
#endif
  }

  if (tokens == NULL) {
    return JSMN_SUCCESS;
  }

#ifdef JSMN_PERMISSIVE
  tokens[parser->toknext - 1].type &= ~JSMN_VALUE;
  tokens[parser->toknext - 1].type |= JSMN_KEY;
#endif

  parser->toksuper = parser->toknext - 1;

  return JSMN_SUCCESS;
}

static
jsmnint_t jsmn_parse_comma(jsmn_parser *parser, jsmntok_t *tokens) {
  /* If a COMMA wasn't expected, strict check because it is a complex enum */
  if (!((parser->expected & JSMN_COMMA) == JSMN_COMMA)) {
    return JSMN_ERROR_INVAL;
  }

  jsmntype_t type = JSMN_UNDEFINED;
  if (tokens == NULL) {
    if (parser->toksuper < (sizeof(jsmnint_t) * 8) &&
        parser->toknext & (1 << parser->toksuper)) {
      type = JSMN_INSD_OBJ;
    }
  } else {
    if (parser->toksuper != JSMN_NEG) {
      type = tokens[parser->toksuper].type;
    }
  }

  if (parser->toksuper != JSMN_NEG) {
    if (type & (JSMN_OBJECT | JSMN_INSD_OBJ)) {
      parser->expected = JSMN_AFTR_COMMA_O;
    } else {
      parser->expected = JSMN_AFTR_COMMA_A;
    }
#ifdef JSMN_PERMISSIVE
  } else {
    parser->expected = JSMN_AFTR_COMMA_R;
#endif
  }

  if (tokens == NULL) {
    return JSMN_SUCCESS;
  }

#ifdef JSMN_PERMISSIVE
  tokens[parser->toknext - 1].type |= JSMN_VALUE;
#endif

  return JSMN_SUCCESS;
}

/**
 * Parse JSON string and fill tokens.
 */
JSMN_API
jsmnint_t jsmn_parse(jsmn_parser *parser, const char *js,
                     const size_t len, jsmntok_t *tokens,
                     const size_t num_tokens) {
  jsmnint_t r;
  jsmnint_t count = parser->toknext;

  char c;
  for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
    c = js[parser->pos];
    switch (c) {
    case '{':
    case '[':
      r = jsmn_parse_container_open(parser, c, tokens, num_tokens);
      if (r != JSMN_SUCCESS) {
        return r;
      }
      count++;
      break;
    case '}':
    case ']':
      r = jsmn_parse_container_close(parser, c, tokens);
      if (r != JSMN_SUCCESS) {
        return r;
      }
      break;
    case '\"':
      r = jsmn_parse_string(parser, js, len, tokens, num_tokens);
      if (r != JSMN_SUCCESS) {
        return r;
      }
      count++;
      break;
    case ':':
      r = jsmn_parse_colon(parser, tokens);
      if (r != JSMN_SUCCESS) {
        return r;
      }
      break;
    case ',':
      r = jsmn_parse_comma(parser, tokens);
      if (r != JSMN_SUCCESS) {
        return r;
      }
      break;
    case ' ':
    case '\t':
    case '\n':
    case '\r':
      break;
#ifndef JSMN_PERMISSIVE
    /* rfc8259: PRIMITIVEs are numbers and booleans */
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
    /* In permissive mode every unquoted value is a PRIMITIVE */
    default:
#endif
      r = jsmn_parse_primitive(parser, js, len, tokens, num_tokens);
      if (r != JSMN_SUCCESS) {
        return r;
      }
      count++;
      break;

#ifndef JSMN_PERMISSIVE
    /* Unexpected char */
    default:
      return JSMN_ERROR_INVAL;
#endif
    }
  }

  if (parser->toksuper != JSMN_NEG) {
    return JSMN_ERROR_PART;
  }

  return count;
}

/**
 * Creates a new parser based over a given buffer with an array of tokens
 * available.
 */
JSMN_API
void jsmn_init(jsmn_parser *parser) {
  parser->pos = 0;
  parser->toknext = 0;
  parser->toksuper = JSMN_NEG;
  parser->expected = JSMN_ROOT_INIT;
}

#endif /* JSMN_HEADER */

#ifdef __cplusplus
}
#endif

#endif /* JSMN_H */
