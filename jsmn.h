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

#ifdef JSMN_STATIC
#define JSMN_API static
#else
#define JSMN_API extern
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

  // Combined elements for valid enum tests
  JSMN_ANY_TYPE  = JSMN_OBJECT | JSMN_ARRAY | JSMN_STRING | JSMN_PRIMITIVE,

  JSMN_OBJ_VAL   = JSMN_OBJECT | JSMN_VALUE,
  JSMN_ARR_VAL   = JSMN_ARRAY  | JSMN_VALUE,
  JSMN_STR_KEY   = JSMN_STRING | JSMN_KEY,
  JSMN_STR_VAL   = JSMN_STRING | JSMN_VALUE,
  JSMN_PRI_VAL   = JSMN_PRIMITIVE | JSMN_VALUE,
#if !defined(JSMN_STRICT)
  JSMN_OBJ_KEY   = JSMN_OBJECT | JSMN_KEY,
  JSMN_ARR_KEY   = JSMN_ARRAY  | JSMN_KEY,
  JSMN_PRI_KEY   = JSMN_PRIMITIVE | JSMN_KEY,
#endif
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
  // Start with parent's first child
  jsmnint_t sibling = parser->toksuper + 1;

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
#ifdef JSMN_STRICT
  // If a primitive wasn't expected
  if (tokens != NULL && !(parser->expected & JSMN_PRIMITIVE)) {
    return JSMN_ERROR_INVAL;
  }
#endif

  jsmntok_t *token;
  jsmnint_t start;

  start = parser->pos;

  for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
    switch (js[parser->pos]) {
#ifndef JSMN_STRICT
    // In strict mode primitive must be followed by "," or "}" or "]"
    case ':':
#endif
    case '\t':
    case '\r':
    case '\n':
    case ' ':
    case ',':
    case ']':
    case '}':
      goto found;
    default:                // to quiet a warning from gcc
      break;
    }
    if (js[parser->pos] < 32 || js[parser->pos] >= 127) {
      parser->pos = start;
      return JSMN_ERROR_INVAL;
    }
  }
#ifdef JSMN_STRICT
  // In strict mode primitive must be followed by a comma/object/array
  parser->pos = start;
  return JSMN_ERROR_PART;
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
  jsmn_fill_token(token, JSMN_PRIMITIVE, start, parser->pos);
#ifdef JSMN_STRICT
  token->type |= JSMN_VALUE;
  parser->expected = (JSMN_DELIMITER | JSMN_CLOSE);
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
#ifdef JSMN_STRICT
  // If a string wasn't expected
  if (tokens != NULL && !(parser->expected & JSMN_STRING)) {
    return JSMN_ERROR_INVAL;
  }
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
#ifdef JSMN_STRICT
      // If the parent type is an object and the previous token is an object or value
      if ((tokens[parser->toksuper].type & JSMN_OBJECT) &&
          (tokens[parser->toknext - 2].type & (JSMN_OBJECT | JSMN_VALUE))) {
        token->type |= JSMN_KEY;
        parser->expected = JSMN_DELIMITER;
      } else {
        token->type |= JSMN_VALUE;
        parser->expected = (JSMN_DELIMITER | JSMN_CLOSE);
      }
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
      case '/':
      case '\\':
      case 'b':
      case 'f':
      case 'r':
      case 'n':
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
#ifdef JSMN_STRICT
      // If an OBJECT or ARRAY (respectively) wasn't expected
      if (!(parser->expected & token->type)) {
        return JSMN_ERROR_INVAL;
      }

      token->type |= JSMN_VALUE;
      if (token->type & JSMN_OBJECT) {
        parser->expected = (JSMN_STRING | JSMN_CLOSE);
      } else {
        parser->expected = (JSMN_ANY_TYPE | JSMN_CLOSE);
      }
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
#ifdef JSMN_STRICT
      if (!(parser->expected & JSMN_CLOSE)) {
        return JSMN_ERROR_INVAL;
      }
#endif
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
#ifdef JSMN_STRICT
      if (parser->toksuper == JSMN_NEG) {
        parser->expected = (JSMN_OBJECT | JSMN_ARRAY);
      } else {
        parser->expected = (JSMN_DELIMITER | JSMN_CLOSE);
      }
#endif
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
    case '\t':
    case '\r':
    case '\n':
    case ' ':
      break;
    case ':':
      if (tokens != NULL && parser->toksuper != JSMN_NEG) {
#ifdef JSMN_STRICT
        // If a DELIMITER wasn't expected or the previous token wasn't a KEY
        if (!(parser->expected & JSMN_DELIMITER) ||
            !(tokens[parser->toknext - 1].type & JSMN_KEY)) {
          return JSMN_ERROR_INVAL;
        }
        parser->expected = JSMN_ANY_TYPE;
#else
        // ':' is not a valid DELIMITER in an ARRAY
        if (tokens[parser->toksuper].type & JSMN_ARRAY) {
          return JSMN_ERROR_INVAL;
        }
#endif
      }
      parser->toksuper = parser->toknext - 1;
      break;
    case ',':
      if (tokens != NULL && parser->toksuper != JSMN_NEG) {
#ifdef JSMN_STRICT
        // If a DELIMITER wasn't expected or the previous token was a KEY
        if (!(parser->expected & JSMN_DELIMITER) ||
            tokens[parser->toknext - 1].type & JSMN_KEY) {
          return JSMN_ERROR_INVAL;
        }
        // If this is in an OBJECT, a STRING KEY must follow a comma
        if (tokens[parser->toksuper].type & JSMN_OBJECT) {
          parser->expected = JSMN_STRING;
        // else this is in an ARRAY which allows ANY_TYPE to follow
        } else {
          parser->expected = JSMN_ANY_TYPE;
        }
#endif
        if (!(tokens[parser->toksuper].type & (JSMN_OBJECT | JSMN_ARRAY))) {
#ifdef JSMN_PARENT_LINKS
          parser->toksuper = tokens[parser->toksuper].parent;
#else
          for (i = parser->toknext - 1; i != JSMN_NEG; i--) {
            if (tokens[i].type & (JSMN_OBJECT | JSMN_ARRAY)) {
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
#ifdef JSMN_STRICT
    // In strict mode primitives are: numbers and booleans
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
    // In non-strict mode every unquoted value is a primitive
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

#ifdef JSMN_STRICT
    // Unexpected char in strict mode
    default:
      return JSMN_ERROR_INVAL;
#endif
    }
  }

  if (tokens != NULL) {
    for (i = parser->toknext - 1; i != JSMN_NEG; i--) {
      // Unmatched opened object or array
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
  parser->expected = (JSMN_OBJECT | JSMN_ARRAY);
}

#endif // JSMN_HEADER

#ifdef __cplusplus
}
#endif

#endif // JSMN_H
