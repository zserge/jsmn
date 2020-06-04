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

/**
 * JSON type identifier. Basic types are:
 */
typedef enum {
  JSMN_UNDEFINED = 0x00,
  JSMN_OBJECT    = 0x01,    //!< Object
  JSMN_ARRAY     = 0x02,    //!< Array
  JSMN_STRING    = 0x04,    //!< String
  JSMN_PRIMITIVE = 0x08,    //!< Other primitive: number, boolean (true/false) or null

  JSMN_KEY       = 0x10,    //!< is a key
  JSMN_VALUE     = 0x20,    //!< is a value

  JSMN_CLOSE     = 0x40,    //!< Close OBJECT '}' or ARRAY ']'
  JSMN_DELIMITER = 0x80,    //!< Colon ':' after KEY, Comma ',' after VALUE

  // Combined elements
  JSMN_CONTAINER = JSMN_OBJECT | JSMN_ARRAY,
  JSMN_ANY_TYPE  = JSMN_OBJECT | JSMN_ARRAY | JSMN_STRING | JSMN_PRIMITIVE,

  JSMN_OBJ_VAL   = JSMN_OBJECT | JSMN_VALUE,
  JSMN_ARR_VAL   = JSMN_ARRAY  | JSMN_VALUE,
  JSMN_STR_KEY   = JSMN_STRING | JSMN_KEY,
  JSMN_STR_VAL   = JSMN_STRING | JSMN_VALUE,
  JSMN_PRI_VAL   = JSMN_PRIMITIVE | JSMN_VALUE,
#ifdef JSMN_PERMISSIVE
  JSMN_OBJ_KEY   = JSMN_OBJECT | JSMN_KEY,
  JSMN_ARR_KEY   = JSMN_ARRAY  | JSMN_KEY,
  JSMN_PRI_KEY   = JSMN_PRIMITIVE | JSMN_KEY,
#endif

  JSMN_PRI_LITERAL  = 0x0100,   //!< true, false, null

  JSMN_PRI_ZERO     = 0x0400,   //!< 0
  JSMN_PRI_DIGIT    = 0x0800,   //!< 1 - 9
  JSMN_PRI_MINUS    = 0x1000,   //!< minus sign, '-'
  JSMN_PRI_PLUS     = 0x2000,   //!< plus sign, '+'
  JSMN_PRI_DECIMAL  = 0x4000,   //!< deminal point '.'
  JSMN_PRI_EXPONENT = 0x8000,   //!< exponent, 'e' or 'E'

  JSMN_PRI_NUMBER   = JSMN_PRI_ZERO | JSMN_PRI_DIGIT,
  JSMN_PRI_SIGN     = JSMN_PRI_PLUS | JSMN_PRI_MINUS,
} jsmntype_t;

enum jsmnerr {
  JSMN_SUCCESS     =  0,
  JSMN_ERROR_NOMEM = -1,    //!< Not enough tokens were provided
  JSMN_ERROR_INVAL = -2,    //!< Invalid character inside JSON string
  JSMN_ERROR_PART  = -3,    //!< The string is not a full JSON packet, more bytes expected
};

typedef unsigned int jsmnint_t;
#define JSMN_NEG ((jsmnint_t)-1)

/**
 * JSON token description.
 */
typedef struct jsmntok_t {
  jsmntype_t type;          //!< type (object, array, string etc.)
  jsmnint_t start;          //!< start position in JSON data string
  jsmnint_t end;            //!< end position in JSON data string
  jsmnint_t size;           //!< number of children
#ifdef JSMN_PARENT_LINKS
  jsmnint_t parent;         //!< parent id
#endif
#ifdef JSMN_NEXT_SIBLING
  jsmnint_t next_sibling;   //!< next sibling id
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
  jsmnint_t toksuper;       //!< superior token node, e.g. parent object or array
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
  jsmntok_t *tok;
  if (parser->toknext >= num_tokens) {
    return NULL;
  }
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

  // Start with parent's first child
  if (parser->toksuper != JSMN_NEG) {
    sibling = parser->toksuper + 1;
  } else {
    sibling = 0;
  }

  // If the first child is the current token
  if (sibling == parser->toknext - 1)
    return;

  // Loop until we find previous sibling
  while (tokens[sibling].next_sibling != JSMN_NEG)
    sibling = tokens[sibling].next_sibling;

  // Set previous sibling's next_sibling to current token
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
  // If a PRIMITIVE wasn't expected
  if (tokens != NULL &&
      !(parser->expected & JSMN_PRIMITIVE)) {
    return JSMN_ERROR_INVAL;
  }

  jsmntok_t *token;
  jsmnint_t start;
  jsmntype_t type;

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
    // If there are no extra JSMN_PRI_* flags
    if ((parser->expected & 0xFF00) == 0) {
      parser->expected = JSMN_PRIMITIVE | JSMN_PRI_MINUS | JSMN_PRI_NUMBER;
    }
    for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
      switch (js[parser->pos]) {
      case '0':
        if (!(parser->expected & JSMN_PRI_ZERO)) {
          parser->pos = start;
          return JSMN_ERROR_INVAL;
        }
        if (type & JSMN_PRI_EXPONENT) {
          parser->expected = JSMN_PRIMITIVE | JSMN_PRI_NUMBER;
        } else if (type & JSMN_PRI_DECIMAL) {
          parser->expected = JSMN_PRIMITIVE | JSMN_PRI_NUMBER |JSMN_PRI_EXPONENT;
        } else if (start == parser->pos ||
            (start + 1 == parser->pos && (type & JSMN_PRI_MINUS))) {
          parser->expected = JSMN_PRIMITIVE | JSMN_PRI_DECIMAL | JSMN_PRI_EXPONENT;
        } else {
          parser->expected = JSMN_PRIMITIVE | JSMN_PRI_NUMBER | JSMN_PRI_DECIMAL | JSMN_PRI_EXPONENT;
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
        if (!(parser->expected & JSMN_PRI_DIGIT)) {
          parser->pos = start;
          return JSMN_ERROR_INVAL;
        }
        if (type & JSMN_PRI_EXPONENT) {
          parser->expected = JSMN_PRIMITIVE | JSMN_PRI_NUMBER;
        } else if (type & JSMN_PRI_DECIMAL) {
          parser->expected = JSMN_PRIMITIVE | JSMN_PRI_NUMBER | JSMN_PRI_EXPONENT;
        } else {
          parser->expected = JSMN_PRIMITIVE | JSMN_PRI_NUMBER | JSMN_PRI_DECIMAL | JSMN_PRI_EXPONENT;
        }
        break;
      case '-':
        if (!(parser->expected & JSMN_PRI_MINUS)) {
          parser->pos = start;
          return JSMN_ERROR_INVAL;
        }
        parser->expected = JSMN_PRIMITIVE | JSMN_PRI_NUMBER;
        if (start == parser->pos) {
          type |= JSMN_PRI_MINUS;
        }
        break;
      case '+':
        if (!(parser->expected & JSMN_PRI_PLUS)) {
          parser->pos = start;
          return JSMN_ERROR_INVAL;
        }
        parser->expected = JSMN_PRIMITIVE | JSMN_PRI_NUMBER;
        break;
      case '.':
        if (!(parser->expected & JSMN_PRI_DECIMAL)) {
          parser->pos = start;
          return JSMN_ERROR_INVAL;
        }
        type |= JSMN_PRI_DECIMAL;
        parser->expected = JSMN_PRIMITIVE | JSMN_PRI_NUMBER;
        break;
      case 'e':
      case 'E':
        if (!(parser->expected & JSMN_PRI_EXPONENT)) {
          parser->pos = start;
          return JSMN_ERROR_INVAL;
        }
        type |= JSMN_PRI_EXPONENT;
        parser->expected = JSMN_PRIMITIVE | JSMN_PRI_SIGN | JSMN_PRI_NUMBER;
        break;
      default:
        if (parser->pos >= len || js[parser->pos] == '\0') {
          parser->pos = start;
          return JSMN_ERROR_PART;
        } else if (start + 1 == parser->pos && (type & JSMN_PRI_MINUS)) {
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
#  ifdef JSMN_PARENT_LINKS
  // If either a PRIMITIVE or DELIMITER were expected and this PRIMITIVE is
  //   following a KEY/VALUE pair, a comma didn't separate the last element
  //   and this element so parser->toksuper needs to be fixed.
  if (tokens != NULL && parser->toknext >= 2 &&
      (parser->expected & JSMN_DELIMITER) &&
      (tokens[parser->toknext - 2].type & JSMN_KEY)) {
    parser->toksuper = tokens[parser->toksuper].parent;
  }
#  endif

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
    default:                // to quiet a warning from gcc
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
    return JSMN_SUCCESS;
  }
  token = jsmn_alloc_token(parser, tokens, num_tokens);
  if (token == NULL) {
    parser->pos = start;
    return JSMN_ERROR_NOMEM;
  }
  jsmn_fill_token(token, type, start, parser->pos);
#ifndef JSMN_PERMISSIVE
  token->type |= JSMN_VALUE;
#else
  if (parser->toksuper != JSMN_NEG &&
      (tokens[parser->toksuper].type & JSMN_KEY)) {
    token->type |= JSMN_VALUE;
  }
#endif
  parser->expected = JSMN_DELIMITER | JSMN_CLOSE;
#ifdef JSMN_PERMISSIVE
#  ifdef JSMN_PARENT_LINKS
  if (parser->toksuper != JSMN_NEG &&
      tokens[parser->toksuper].parent == JSMN_NEG) {
    parser->expected |= JSMN_ANY_TYPE;
  }
#  else
  if (parser->toksuper != JSMN_NEG) {
    jsmnint_t i;
    for (i = parser->toksuper; i != JSMN_NEG; i--) {
      if (tokens[i].type & JSMN_CONTAINER) {
        break;
      }
      if (i != 0) {
        continue;
      }
      parser->expected |= JSMN_ANY_TYPE;
    }
  }
#  endif
#endif
#ifdef JSMN_PARENT_LINKS
  token->parent = parser->toksuper;
#endif
#ifdef JSMN_NEXT_SIBLING
  jsmn_next_sibling(parser, tokens);
#endif
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
  // If a STRING wasn't expected
  if (tokens != NULL &&
      !(parser->expected & JSMN_STRING)) {
    return JSMN_ERROR_INVAL;
  }

#ifdef JSMN_PERMISSIVE
#  ifdef JSMN_PARENT_LINKS
  // If both a PRIMITIVE and DELIMITER were expected and this STRING is
  //   following a KEY/VALUE pair, a comma didn't separate the last element
  //   and this element so parser->toksuper needs to be fixed.
  if (tokens != NULL && parser->toknext >= 2 &&
      (parser->expected & JSMN_DELIMITER) &&
      (tokens[parser->toknext - 2].type & JSMN_KEY)) {
    parser->toksuper = tokens[parser->toksuper].parent;
  }
#  endif
#endif

  jsmntok_t *token;

  char c;
  jsmnint_t i, start = parser->pos;

  parser->pos++;

  // Skip starting quote
  for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
    c = js[parser->pos];

    // Quote: end of string
    if (c == '\"') {
      if (tokens == NULL) {
        return JSMN_SUCCESS;
      }
      token = jsmn_alloc_token(parser, tokens, num_tokens);
      if (token == NULL) {
        parser->pos = start;
        return JSMN_ERROR_NOMEM;
      }
      jsmn_fill_token(token, JSMN_STRING, start + 1, parser->pos);
#ifndef JSMN_PERMISSIVE
      // If the parent type is an OBJECT and the previous token is an OBJECT or VALUE
      if ((tokens[parser->toksuper].type & JSMN_OBJECT) &&
          (tokens[parser->toknext - 2].type & (JSMN_OBJECT | JSMN_VALUE))) {
        token->type |= JSMN_KEY;
        parser->expected = JSMN_DELIMITER;
      } else {
        token->type |= JSMN_VALUE;
        parser->expected = JSMN_DELIMITER | JSMN_CLOSE;
      }
#else
      // If the previous token is an KEY
      if (parser->toknext >= 2 &&
          (tokens[parser->toknext - 2].type & JSMN_KEY)) {
        token->type |= JSMN_VALUE;
      }
      parser->expected = JSMN_ANY_TYPE | JSMN_DELIMITER | JSMN_CLOSE;
#endif
#ifdef JSMN_PARENT_LINKS
      token->parent = parser->toksuper;
#endif
#ifdef JSMN_NEXT_SIBLING
      jsmn_next_sibling(parser, tokens);
#endif
      return JSMN_SUCCESS;
    }

    // Backslash: Quoted symbol expected
    if (c == '\\' && parser->pos + 1 < len) {
      parser->pos++;
      switch (js[parser->pos]) {
      // Allowed escaped symbols
      case '\"':
      case '\\':
      case '/':
      case 'b':
      case 'f':
      case 'n':
      case 'r':
      case 't':
        break;
      // Allows escaped symbol \uXXXX
      case 'u':
        parser->pos++;
        for (i = 0; i < 4 && parser->pos < len && js[parser->pos] != '\0'; i++) {
          // If it isn't a hex character we have an error
          if (!((js[parser->pos] >= 48 && js[parser->pos] <= 57) ||   // 0-9
                (js[parser->pos] >= 65 && js[parser->pos] <= 70) ||   // A-F
                (js[parser->pos] >= 97 && js[parser->pos] <= 102))) { // a-f
            parser->pos = start;
            return JSMN_ERROR_INVAL;
          }
          parser->pos++;
        }
        parser->pos--;
        break;
      // Unexpected symbol
      default:
        parser->pos = start;
        return JSMN_ERROR_INVAL;
      }
    }
  }
  parser->pos = start;
  return JSMN_ERROR_PART;
}

/**
 * Parse JSON string and fill tokens.
 */
JSMN_API
jsmnint_t jsmn_parse(jsmn_parser *parser, const char *js,
                     const size_t len, jsmntok_t *tokens,
                     const size_t num_tokens) {
  jsmnint_t r;
  jsmnint_t i;
  jsmntok_t *token;
  jsmnint_t count = parser->toknext;

  char c;
  jsmntype_t type;
  for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
    c = js[parser->pos];
    switch (c) {
    case '{':
    case '[':
      count++;
      if (tokens == NULL) {
        break;
      }
      token = jsmn_alloc_token(parser, tokens, num_tokens);
      if (token == NULL) {
        return JSMN_ERROR_NOMEM;
      }
      token->type = (c == '{' ? JSMN_OBJECT : JSMN_ARRAY);
      // If an OBJECT or ARRAY (respectively) wasn't expected
      if (!(parser->expected & token->type)) {
        return JSMN_ERROR_INVAL;
      }

      token->type |= JSMN_VALUE;
#ifndef JSMN_PERMISSIVE
      if (token->type & JSMN_OBJECT) {
        parser->expected = JSMN_STRING | JSMN_CLOSE;
      } else {
        parser->expected = JSMN_ANY_TYPE | JSMN_CLOSE;
      }
#else
      parser->expected = JSMN_ANY_TYPE | JSMN_CLOSE;
#endif
      if (parser->toksuper != JSMN_NEG) {
        tokens[parser->toksuper].size++;
#ifdef JSMN_PARENT_LINKS
        token->parent = parser->toksuper;
#endif
#ifdef JSMN_NEXT_SIBLING
        jsmn_next_sibling(parser, tokens);
#endif
      }
      token->start = parser->pos;
      parser->toksuper = parser->toknext - 1;
      break;
    case '}':
    case ']':
      if (tokens == NULL) {
        break;
      }
      if (!(parser->expected & JSMN_CLOSE)) {
        return JSMN_ERROR_INVAL;
      }
      type = (c == '}' ? JSMN_OBJECT : JSMN_ARRAY);
#ifdef JSMN_PARENT_LINKS
      if (parser->toknext < 1) {
        return JSMN_ERROR_INVAL;
      }
      token = &tokens[parser->toknext - 1];
      for (;;) {
        if (token->start != JSMN_NEG && token->end == JSMN_NEG) {
          if (!(token->type & type)) {
            return JSMN_ERROR_INVAL;
          }
          token->end = parser->pos + 1;
          parser->toksuper = token->parent;
          break;
        }
        if (token->parent == JSMN_NEG) {
          if (!(token->type & type) || parser->toksuper == JSMN_NEG) {
            return JSMN_ERROR_INVAL;
          }
          break;
        }
        token = &tokens[token->parent];
      }
#else
      for (i = parser->toknext - 1; i != JSMN_NEG; i--) {
        token = &tokens[i];
        if (token->start != JSMN_NEG && token->end == JSMN_NEG) {
          if (!(token->type & type)) {
            return JSMN_ERROR_INVAL;
          }
          parser->toksuper = JSMN_NEG;
          token->end = parser->pos + 1;
          break;
        }
      }
      // Error if unmatched closing bracket
      if (i == JSMN_NEG) {
        return JSMN_ERROR_INVAL;
      }
      for (; i != JSMN_NEG; i--) {
        token = &tokens[i];
        if (token->start != JSMN_NEG && token->end == JSMN_NEG) {
          parser->toksuper = i;
          break;
        }
      }
#endif
      if (parser->toksuper == JSMN_NEG) {
#ifndef JSMN_PERMISSIVE
        parser->expected = JSMN_CONTAINER;
#else
        tokens[parser->toknext - 1].type |= JSMN_VALUE;
        parser->expected = JSMN_ANY_TYPE;
#endif
      } else {
        parser->expected = JSMN_DELIMITER | JSMN_CLOSE;
      }
      break;
    case '\"':
      r = jsmn_parse_string(parser, js, len, tokens, num_tokens);
      if (r != JSMN_SUCCESS) {
        return r;
      }
      count++;
      if (tokens != NULL && parser->toksuper != JSMN_NEG) {
        tokens[parser->toksuper].size++;
      }
      break;
    case ' ':
    case '\t':
    case '\n':
    case '\r':
      break;
    case ':':
      // If a DELIMITER wasn't expected
      if (!(parser->expected & JSMN_DELIMITER)) {
        return JSMN_ERROR_INVAL;
      }
#ifndef JSMN_PERMISSIVE
      if (tokens != NULL &&
          // Only simple single allowed
          (parser->toksuper == JSMN_NEG ||
           // If the previous token wasn't a KEY
           !(tokens[parser->toknext - 1].type & JSMN_KEY))) {
        return JSMN_ERROR_INVAL;
      }
#else
      tokens[parser->toknext - 1].type |= JSMN_KEY;
#endif
      parser->expected = JSMN_ANY_TYPE;
      parser->toksuper = parser->toknext - 1;
      break;
    case ',':
      if (tokens != NULL && parser->toksuper != JSMN_NEG) {
        // If a DELIMITER wasn't expected
        if (!(parser->expected & JSMN_DELIMITER)) {
          return JSMN_ERROR_INVAL;
        }
#ifndef JSMN_PERMISSIVE
        // If the previous token was a KEY
        if (tokens[parser->toknext - 1].type & JSMN_KEY) {
          return JSMN_ERROR_INVAL;
        }
        // If this is in an OBJECT, a STRING KEY must follow a comma
        if (tokens[parser->toksuper].type & JSMN_OBJECT) {
          parser->expected = JSMN_STRING;
        // else this is in an ARRAY which allows ANY_TYPE to follow
        } else {
          parser->expected = JSMN_ANY_TYPE;
        }
#else
        // The previous token
        tokens[parser->toknext - 1].type |= JSMN_VALUE;
        parser->expected = JSMN_ANY_TYPE;
#endif
        if (!(tokens[parser->toksuper].type & JSMN_CONTAINER)) {
#ifdef JSMN_PARENT_LINKS
          parser->toksuper = tokens[parser->toksuper].parent;
#else
          for (i = parser->toknext - 1; i != JSMN_NEG; i--) {
            if (tokens[i].type & JSMN_CONTAINER) {
              if (tokens[i].start != JSMN_NEG && tokens[i].end == JSMN_NEG) {
                parser->toksuper = i;
                break;
              }
            }
          }
#endif
        }
      }
      break;
#ifndef JSMN_PERMISSIVE
    // rfc8259: PRIMITIVEs are numbers and booleans
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
    // In permissive mode every unquoted value is a PRIMITIVE
    default:
#endif
      r = jsmn_parse_primitive(parser, js, len, tokens, num_tokens);
      if (r != JSMN_SUCCESS) {
        return r;
      }
      count++;
      if (tokens != NULL && parser->toksuper != JSMN_NEG) {
        tokens[parser->toksuper].size++;
      }
      break;

#ifndef JSMN_PERMISSIVE
    // Unexpected char
    default:
      return JSMN_ERROR_INVAL;
#endif
    }
  }

  if (tokens != NULL) {
    for (i = parser->toknext - 1; i != JSMN_NEG; i--) {
      // Unmatched opened OBJECT or ARRAY
      if (tokens[i].start != JSMN_NEG && tokens[i].end == JSMN_NEG) {
        return JSMN_ERROR_PART;
      }
    }
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
#ifndef JSMN_PERMISSIVE
  parser->expected = JSMN_CONTAINER;
#else
  parser->expected = JSMN_ANY_TYPE;
#endif
}

#endif // JSMN_HEADER

#ifdef __cplusplus
}
#endif

#endif // JSMN_H
