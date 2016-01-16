#ifndef __JSMN_ITERATOR_H__
#define __JSMN_ITERATOR_H__

#include "jsmn.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Error return codes for jsmn iterator
 */
enum {
  /* Input parameter error */
  JSMNITER_ERR_PARAMETER  = -1,
  /* JSMN index doesn't point at an Array/Object */
  JSMNITER_ERR_TYPE       = -2,  
  /* Group item misses string identifier */
  JSMNITER_ERR_NOIDENT    = -3,
  /* Broken JSON */
  JSMNITER_ERR_BROKEN     = -4,
};


/**
 * Struct with state information for jsmn iterator
 * - When the no more items for iterator the parser_pos value will point to 
 *   JSMN index for next object after current Array/Object
 */
typedef struct {
  jsmntok_t *jsmn_tokens;
  unsigned int jsmn_len;
  unsigned int parent_pos;
  unsigned int parser_pos;
  unsigned int index;
} jsmn_iterator_t;


/**
 * @brief Locates last JSMN index for current Array/Group
 * @details Iterates over JSMN Array/Object until last item is found
 * 
 * @param jsmn_tokens   JSMN tokens
 * @param jsmn_len      JSMN token count
 * @param parser_pos    Current JSMN token
 *
 * @return  < 0 - Error has occured, corresponds to one of JSMNITER_ERR_*
 *          >=0 - Last JSMN index for Array/Object
 */
int jsmn_find_end( jsmntok_t *jsmn_tokens, unsigned int jsmn_len, unsigned int parser_pos );


/**
 * @brief Initialize iterator
 * @details Set initial value for iterator struct
 * 
 * @param iterator      Iterator struct
 * @param jsmn_tokens   JSMN tokens
 * @param jsmn_len      JSMN token count
 * @param parser_pos    Current JSMN token
 * 
 * @return  < 0 - Error has occured, corresponds to one of JSMNITER_ERR_*
 *          >=0 - Ok
 */
int jsmn_iterator_init( jsmn_iterator_t *iterator, jsmntok_t *jsmn_tokens, unsigned int jsmn_len, 
                        unsigned int parser_pos );


/**
 * @brief Get next item in JSMN Array/Object
 * @details Gets JSMN position for next identifier and value in Array/Object
 * 
 * @param iterator            Iterator struct
 * @param jsmn_identifier     Return pointer for identifier, NULL for Array
 * @param jsmn_value          Return pointer for value
 * @param next_value_index    Possible to indicate where next value begins, allows determine end of sub
 *                            Array/Object withouth manually searching for it
 * 
 * @return  < 0 - Error has occured, corresponds to one of JSMNITER_ERR_*
 *            0 - No more values
 *          > 0 - Value (and identifier) has been returned
 */
int jsmn_iterator_next( jsmn_iterator_t *iterator, jsmntok_t **jsmn_identifier, jsmntok_t **jsmn_value, 
                        unsigned int next_value_index );


#ifdef __cplusplus
}
#endif

#endif /*__JSMN_ITERATOR_H__*/
