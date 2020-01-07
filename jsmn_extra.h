
#include "jsmn.h"

//call a function for each key in an object or value in an array
//if the callback function returns a value other than 0 then the rest of the 
//values are skipped
JSMN_API void jsmn_foreach_in_compound(const char *js, 
                                       jsmntok_t *tokens, 
                                       int num_tokens, 
                                       int compound_token_index, 
                                       int (*cb)(const char *js, 
                                                 jsmntok_t *tokens, 
                                                 int key_token_index, 
                                                 void *ctx),
                                       void *ctx) {
    if (cb == NULL) 
        return;
    if (tokens[compound_token_index].type != JSMN_OBJECT &&
        tokens[compound_token_index].type != JSMN_ARRAY) 
        return;
    int is_object = tokens[compound_token_index].type == JSMN_OBJECT;
    int key_token_index = compound_token_index + 1;
    int value_token_index = key_token_index;
    if (is_object) value_token_index++;
    while (key_token_index < num_tokens) {
        if (tokens[key_token_index].start > tokens[compound_token_index].end)
            break;
        if (cb(js, tokens, key_token_index, ctx) != 0)
            return;
        if (tokens[value_token_index].type == JSMN_OBJECT
            || tokens[value_token_index].type == JSMN_ARRAY) {
            int value_token_end = tokens[value_token_index].end;
            int i = key_token_index;
            while (i < num_tokens) {
                if (tokens[i].start > value_token_end)
                    break;
                i++;
            }
            if (i == num_tokens)
                break;
            else {
                key_token_index = i;
                value_token_index = key_token_index;
                if (is_object) value_token_index++;
            }
        } else {
            key_token_index++;
            if (is_object) key_token_index++;
            value_token_index = key_token_index;
            if (is_object) value_token_index++;
        }
    }
}