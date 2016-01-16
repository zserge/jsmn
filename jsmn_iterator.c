#include "jsmn_iterator.h"


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
int jsmn_iterator_find_end( jsmntok_t *jsmn_tokens, unsigned int jsmn_len, unsigned int parser_pos ) {
  unsigned int child_count;
  unsigned int child_index;
  int parent_end;


  /* No tokens */
  if (!jsmn_tokens)
    return JSMNITER_ERR_PARAMETER;
  /* pos outside tokens */
  if (parser_pos >= jsmn_len)
    return JSMNITER_ERR_PARAMETER;
  /* Not an Array/Object */
  if (jsmn_tokens[parser_pos].type != JSMN_ARRAY && 
      jsmn_tokens[parser_pos].type != JSMN_OBJECT)
    return JSMNITER_ERR_TYPE;

  /* End position for Array/Object */
  parent_end = jsmn_tokens[parser_pos].end;

  /* First child item */
  child_index = parser_pos + 1;

  /* Count number of children we need to iterate */
  child_count = jsmn_tokens[parser_pos].size * (jsmn_tokens[parser_pos].type == JSMN_OBJECT ? 2 : 1);

  /* Loop until end of current Array/Object */
  while(child_index < jsmn_len && jsmn_tokens[child_index].start <= parent_end && child_count >= 0) {
    /* Add item count in sub Arrays/Objects that we need to skip */
    if (jsmn_tokens[child_index].type == JSMN_ARRAY)
      child_count += jsmn_tokens[child_index].size;
    else if (jsmn_tokens[child_index].type == JSMN_OBJECT)
      child_count += jsmn_tokens[child_index].size * 2;

    child_count--;
    child_index++;
  }

  /* Validate that we have visited correct number of children */
  if (child_count != 0)
    return JSMNITER_ERR_BROKEN;

  /* Points to last index inside Array/Object */
  return (int)child_index - 1;
}


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
int jsmn_iterator_init(jsmn_iterator_t *iterator, jsmntok_t *jsmn_tokens, unsigned int jsmn_len, unsigned int parser_pos) {
  /* No tokens */
  if (!jsmn_tokens)
    return JSMNITER_ERR_PARAMETER;
  /* pos outside tokens */
  if (parser_pos >= jsmn_len)
    return JSMNITER_ERR_PARAMETER;
  /* Not an Array/Object */
  if (jsmn_tokens[parser_pos].type != JSMN_ARRAY && 
      jsmn_tokens[parser_pos].type != JSMN_OBJECT)
    return JSMNITER_ERR_TYPE;

  /* Save jsmn pointer */
  iterator->jsmn_tokens = jsmn_tokens;
  iterator->jsmn_len    = jsmn_len;

  iterator->parent_pos  = parser_pos;
  iterator->parser_pos  = parser_pos;

  iterator->index  = 0;
  return 0;
}


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
 int jsmn_iterator_next(jsmn_iterator_t *iterator, jsmntok_t **jsmn_identifier, jsmntok_t **jsmn_value, 
                        unsigned int next_value_index) {
  unsigned int is_object;
  jsmntok_t *parent_item;
  jsmntok_t *current_item;
  jsmntok_t *jsmn_tokens = iterator->jsmn_tokens;
  unsigned int jsmn_len = iterator->jsmn_len;

  /* Parser position is outside JSMN tokens array */
  if (iterator->parser_pos > iterator->jsmn_len)
    return JSMNITER_ERR_BROKEN;

  parent_item  = &jsmn_tokens[iterator->parent_pos];

  /* parser_position is at the end of JSMN token array or points outside parent Array/Object */
  if (iterator->parser_pos == iterator->jsmn_len || jsmn_tokens[iterator->parser_pos].start > parent_item->end) {
    if (iterator->index != parent_item->size)
      return JSMNITER_ERR_BROKEN;
    return 0;
  }

  current_item = &jsmn_tokens[iterator->parser_pos];


  /* Are we in an Object */
  is_object = (parent_item->type == JSMN_OBJECT ? 1 : 0);

  /* Is it item we only need jump one to the next index */
  if (iterator->parser_pos == iterator->parent_pos) {
    next_value_index = iterator->parser_pos + 1;
  }
  /* For items that isn't Array/Object we only need to take the next value */
  else if (current_item->type != JSMN_ARRAY &&
           current_item->type != JSMN_OBJECT) {
    next_value_index = iterator->parser_pos + 1;
  }
  /* Check if next_value_index is correct, else we need to calculate it ourself */
  else if (next_value_index == 0 ||
           next_value_index > jsmn_len || 
           next_value_index <= iterator->parser_pos ||
           current_item->end < jsmn_tokens[next_value_index - 1].end ||
           (next_value_index < jsmn_len && current_item->end >= jsmn_tokens[next_value_index].start)) {
    /* Find end index for the Array/Object manually */
    int return_pos = jsmn_iterator_find_end(jsmn_tokens, jsmn_len, iterator->parser_pos);

    /* Error, bail out */
    if (return_pos < 0)
      return return_pos;
  
    /* Update position to the next item token */
    next_value_index = (unsigned int)(return_pos) + 1;

    /* Outside valid array */
    if (next_value_index> jsmn_len)
      return JSMNITER_ERR_BROKEN;
    /* Earlier than current value (not valid jsmn tree) */
    if (next_value_index <= iterator->parser_pos)
      return JSMNITER_ERR_BROKEN;
    /* Previus item is NOT inside current Array/Object */
    if (jsmn_tokens[next_value_index - 1].end > current_item->end)
      return JSMNITER_ERR_BROKEN;
    /* Not last item and next item is NOT outside Array/Object */
    if (next_value_index < jsmn_len && current_item->end > jsmn_tokens[next_value_index].start)
      return JSMNITER_ERR_BROKEN;
  }

  /* Update parser position */
  iterator->parser_pos = next_value_index;

  /* Parser position is outside JSMN tokens array */
  if (iterator->parser_pos > iterator->jsmn_len)
    return JSMNITER_ERR_BROKEN;

  /* parser_position is at the end of JSMN token array or points outside parent Array/Object */
  if (iterator->parser_pos == iterator->jsmn_len || jsmn_tokens[iterator->parser_pos].start > parent_item->end) {
    if (iterator->index != parent_item->size)
      return JSMNITER_ERR_BROKEN;
    return 0;
  }

  current_item = &jsmn_tokens[iterator->parser_pos];

  /* We have identifier, read it */
  if (is_object) {
    /* Must be string */
    if (current_item->type != JSMN_STRING)
      return JSMNITER_ERR_NOIDENT;
    /* Ensure that we have next token */
    if (iterator->parser_pos + 1 >= jsmn_len)
      return JSMNITER_ERR_BROKEN;
    /* Missing identifier pointer */
    if (!jsmn_identifier)
      return JSMNITER_ERR_PARAMETER;
    
    /* Set identifier and update current pointer to value item */
    *jsmn_identifier = current_item;
    current_item = &jsmn_tokens[++iterator->parser_pos];
  }
  /* Clear identifier if is set */
  else if (jsmn_identifier) {
    *jsmn_identifier = NULL;
  }
  
  /* Set value */
  *jsmn_value = current_item;

  /* Increase the index and return it as the positive value */
  iterator->index++;
  return (int)iterator->index;
}

