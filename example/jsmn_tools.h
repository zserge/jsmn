/**
 * @file jsmn_tools.h
 *
 * @brief   An Auxiliary Header for JSMN Library Enabling Enhanced JSON
 *          Structure Enumeration
 *
 * This header file, designed to complement the jsmn.h library, is tailored
 * for use in conjunction with a specially defined JSMN_PARENT_CNT during the
 * compilation of jsmn.h. This subtle customisation allows for refined and
 * advanced JSON structure traversal, making it possible to iteratively explore
 * nested sub-objects within the JSON hierarchy.
 *
 */

#ifndef __JSNM_TOOLS_H__
#define __JSNM_TOOLS_H__

#define JSMN_HEADER
#define JSMN_PARENT_LINKS
#define JSMN_PARENT_CNT
#include "../jsmn.h"


#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Structure for representing a key-value pair in a JSON object.
 *
 * This structure is used to associate a JSON object key (a string) with its
 * corresponding value (a jsmntok_t token). It is employed to store and manage
 * key-value pairs extracted from a JSON object during parsing.
 *
 * @param key   A pointer to the key string.
 * @param value A pointer to the jsmntok_t token of the value.
 */
typedef struct jsmn_key_val_s {
    const char * key ;
    jsmntok_t * value ;
} jsmn_key_val_t ;

/**
 * Enumerate and extract key-value pairs from a JSON object.
 *
 * This function takes a JSON object represented by a jsmntok_t pointer
 * and extracts its key-value pairs. It then updates an array of jsmn_key_val_t
 * structures, associating the keys with their corresponding values.
 *
 * This function will not traverse nested structures or arrays.
 *
 *
 * @param js          A pointer to the JSON string.
 * @param t           A pointer to the JSON object token.
 * @param count       The number of tokens in the JSON object.
 * @param out         An array of jsmn_key_val_t structures to store the key-value pairs.
 * @param out_count   The size of the 'out' array.
 *
 * @return 0 if successful, -1 if an error occurs (e.g., unexpected JSON structure).
 */
int jsmn_object_enum (const char *js, jsmntok_t *t, size_t count,
                            jsmn_key_val_t * out, size_t out_count) ;

/**
 * Get the next JSON token in a sequence.
 *
 * This function takes the current token pointer 't' and returns a pointer
 * to the next token in a JSON token sequence. If 't' is NULL, it returns
 * the first token in the sequence.
 *
 * This function will not traverse nested structures or arrays.
 *
 * @param first A pointer to the first token in the JSON token sequence.
 * @param t     A pointer to the current token.
 *
 * @return A pointer to the next token in the sequence or NULL if there are no more tokens.
 */
jsmntok_t * jsmn_get_next(jsmntok_t *first, jsmntok_t *t);

/**
 * Compare a JSON token with a given string.
 *
 * This function compares the content of a JSON token with a provided string.
 * It checks if the token represents a string and if the string's length matches
 * the length of the provided string. Additionally, it verifies if the actual content
 * of the token matches the provided string.
 *
 * @param json A pointer to the JSON string.
 * @param t A pointer to the JSON token to be compared.
 * @param s A pointer to the string for comparison.
 *
 * @return 0 if the token content matches the provided string, -1 otherwise.
 */
int jsmn_eq(const char *json, jsmntok_t *t, const char *s);

/**
 * Copy the content of a JSON string token to a character buffer.
 *
 * This function copies the content of a JSON string token to a character buffer.
 * It uses snprintf to ensure proper handling of the destination buffer size
 * and takes care of extracting the substring based on the token's start and end indices.
 *
 * @param js A pointer to the JSON string.
 * @param t A pointer to the JSON object token.
 * @param sz A pointer to the destination buffer.
 * @param size The size of the destination buffer.
 *
 * @return The number of characters written to the destination buffer (excluding the null terminator).
 *         If the return value is greater than or equal to 'size', it indicates truncation.
 */
int jsmn_copy_str (const char *js, jsmntok_t *t, char * sz, size_t size);


#ifdef __cplusplus
}
#endif

#endif /* __JSNM_TOOLS_H__ */
