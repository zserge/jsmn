#ifndef JSMN_UTILS_H_
#define JSMN_UTILS_H_

#include <stdarg.h>
#include <stdint.h>

#include "jsmn_defines.h"

#include "jsmn.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return a pointer to jsmn error message
 *
 * @param[in] errno jsmn error number
 * @return const char* jsmn error message
 */
const char *jsmn_strerror(jsmnerr errno);

/**
 * @brief Tokenizes JSON string
 *
 * @param[in] json JSON String
 * @param[in] json_len Length of JSON String
 * @param[out] rv Return Value
 * @return Allocated jsmntok_t array pointer
 */
jsmntok_t *jsmn_tokenize(const char *json, const size_t json_len, jsmnint_t *rv);

/**
 * @brief Tokenize JSON string
 *
 * @param[out] tokens Pointer to preallocated Tokens
 * @param[in] num_tokens Number of Tokens
 * @param[in] json JSON String
 * @param[in] json_len Length of JSON String
 * @return Return Value
 */
jsmnint_t jsmn_tokenize_noalloc(jsmntok_t *tokens, const uint32_t num_tokens, const char *json, const size_t json_len);

/**
 * @brief String comparison between token and string
 *
 * @param[in] json JSON String
 * @param[in] tok Token to compare
 * @param[in] s String to complare
 * @return 0 when token string and s are equal, -1 otherwise
 */
int jsmn_streq(const char *json, const jsmntok_t *tok, const char *s);

/**
 * @brief Find the previous sibling of token at position t
 *
 * @param[in] tokens jsmn tokens
 * @param[in] t the position of the token
 * @return jsmnint_t the position of t's previous sibling, else JSMN_NEG
 */
jsmnint_t jsmn_get_prev_sibling(const jsmntok_t *tokens, const jsmnint_t t);

/**
 * @brief Find the next sibling of token at position t
 *
 * @param[in] tokens jsmn tokens
 * @param[in] t the position of the token
 * @return jsmnint_t the position of t's next sibling, else JSMN_NEG
 */
jsmnint_t jsmn_get_next_sibling(const jsmntok_t *tokens, const jsmnint_t t);

/**
 * @brief Look for a value in a JSON string
 *
 * @param[in] json json string
 * @param[in] tokens jsmn tokens
 * @param[in] num_keys number of keys
 * @return jsmnint_t position of value requested
 */
jsmnint_t jsmn_lookup(const char *json, const jsmntok_t *tokens, const size_t num_keys, ...);

/**
 * @brief Print an extremely verbose description of JSON string
 *
 * @param[in] json JSON String
 * @param[in] len Length of JSON String
 */
void jsmn_explodeJSON(const char *json, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* JSMN_UTILS_H_ */
