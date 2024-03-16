/*
This is my first contribution to a open source project so i have
no idea what to do here
Copyright (c) 2024 VishnuIP   regarding this use my name or the 
name of the original author of the repo.
Or even it is needed to put it here.
Please correct this accordingly.
Also please edit out this. @Serge A. Zaitsev
/*
 * MIT License
 *
 * Copyright (c) 2024 VishnuIP   
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

#include "jsmn_organizer.h"



static void print_selected_key_value_pairs
						(
						const char *json,
						const jsmntok_t *tokens,
						int start_token_index,
						int end_token_index
						);




void jsmn_organize
						(
						jsmntok_t * tokens_received_after_parsing,
						int num_tokens,
						jsmn_organized_t * result
						)
{


	result->number_of_objects = 0;
	result->number_of_arrays = 0;
	result->number_of_indexes_to_neglet = 0;

	for(int i = 0; i < num_tokens; i++)
	{
		jsmntok_t *token = &tokens_received_after_parsing[i];

		if(token->type == JSMN_OBJECT){

			if(token->start == 0){
				result->objects[result->number_of_objects].object_type = MAIN_OBJECT;
			}else{
				result->objects[result->number_of_objects].object_type = SUB_OBJECT;
				result->objects[result->number_of_objects].key_token_for_sub_object_index = i-1;
			}
			result->objects[result->number_of_objects].object_boundary_token_index = i;


			if(token->start != 0 ){
		   int end_pos   = token->end;

		   for(int index = i+1; index < num_tokens; index++){
			   if(tokens_received_after_parsing[index].end < end_pos ){
				   result->indexes_to_neglet[result->number_of_indexes_to_neglet] = index;
				   result->number_of_indexes_to_neglet++;
			   }
		   }
			}

			result->number_of_objects++;
		}



		if(token->type == JSMN_ARRAY){

		   result->arrays[result->number_of_arrays].array_boundary_token_index = i;
		   result->arrays[result->number_of_arrays].key_token_for_array_index = i-1;


		   int end_pos   = token->end;
		   for(int index = i+1; index < num_tokens; index++){
			   if(tokens_received_after_parsing[index].end < end_pos ){
				   result->indexes_to_neglet[result->number_of_indexes_to_neglet] = index;
				   result->number_of_indexes_to_neglet++;
			   }
		   }

		   result->number_of_arrays++;
		}



		// remove the indexes that are just inside an JSMN_ARRAY or SUB_OBJECT


	}
}


void jsmn_print_organized_key_value_pairs
						(
						jsmn_organized_t * result,
						const char *json,
						const jsmntok_t *tokens,
						int num_tokens
						)
/*
 * Use this function
 * Key: name, Value: John
 * Key: age, Value: 30
 * Key: isStudent, Value: false
 * Key: marks, Value: ["address": {"city": "New York","country": "USA"}, 90, 85, 95]
 * Key: address, Value: {"city": "New York","country": "USA"}
 * Key: languages, Value: ["English", "Spanish"]
 */
{

	for(int i = 1; i < num_tokens; i++){
		for(int j = 0; j < result->number_of_indexes_to_neglet; j++){
			if(result->indexes_to_neglet[j] == i){
				i++;
			}
		}

		if(i < num_tokens){
			print_selected_key_value_pairs(json, tokens, i , i+1);
			i++;
		}
	}
}

static void print_selected_key_value_pairs
						(
						const char *json,
						const jsmntok_t *tokens,
						int start_token_index,
						int end_token_index
						)
/*
 * Internal funtction
 */
{
    for (int i = start_token_index; i < end_token_index; i += 2) {
        // Extract key
        int key_start = tokens[i].start;
        int key_end = tokens[i].end;
        printf("Key: ");
        for (int j = key_start; j < key_end; j++) {
            printf("%c", json[j]);
        }
        printf(", ");

        // Extract value
        int value_start = tokens[i + 1].start;
        int value_end = tokens[i + 1].end;
        printf("Value: ");
        for (int j = value_start; j < value_end; j++) {
            printf("%c", json[j]);
        }
        printf("\n");
    }
}






/*
 * Testing functions only - not useful in a meaningful way
 */
void print_all_key_value_pairs
						(
						const char *json,
						const jsmntok_t *tokens,
						int num_tokens
						)
/*
 * Function for testing
 * not usefull
 */
{
	for (int i = 1; i < num_tokens; i += 2) {
		// Extract key
		int key_start = tokens[i].start;
		int key_end = tokens[i].end;
		printf("Key: ");
		for (int j = key_start; j < key_end; j++) {
			printf("%c", json[j]);
		}
		printf(", ");

		// Extract value
		int value_start = tokens[i + 1].start;
		int value_end = tokens[i + 1].end;
		printf("Value: ");
		for (int j = value_start; j < value_end; j++) {
			printf("%c", json[j]);
		}
		printf("\n");
	}
}

void print_all_tokens_and_values
						(
						const char *json,
						const jsmntok_t *tokens,
						int num_tokens
						)
/*
 * SO BASICALLY THIS IS NOT USEFULL
 *
 * Token: 0 , String: {"name": "John","age": 30,"isStudent": false,"marks": ["address": {"city": "New York","country": "USA"}, 90, 85, 95],"address": {"city": "New York","country": "USA"},"languages": ["English", "Spanish"]}
Token: 1 , String: name
Token: 2 , String: John
Token: 3 , String: age
Token: 4 , String: 30
Token: 5 , String: isStudent
Token: 6 , String: false
Token: 7 , String: marks
Token: 8 , String: ["address": {"city": "New York","country": "USA"}, 90, 85, 95]
Token: 9 , String: address
Token: 10 , String: {"city": "New York","country": "USA"}
Token: 11 , String: city
Token: 12 , String: New York
Token: 13 , String: country
Token: 14 , String: USA
Token: 15 , String: 90
Token: 16 , String: 85
Token: 17 , String: 95
Token: 18 , String: address
Token: 19 , String: {"city": "New York","country": "USA"}
Token: 20 , String: city
Token: 21 , String: New York
Token: 22 , String: country
Token: 23 , String: USA
Token: 24 , String: languages
Token: 25 , String: ["English", "Spanish"]
Token: 26 , String: English
Token: 27 , String: Spanish
 *
 */
{
	for(int i = 0; i < num_tokens; i++){
        int _start = tokens[i].start;
        int _end = tokens[i].end;
        printf("Token: %d , String: ",i);
        for (int j = _start; j < _end; j++) {
            printf("%c", json[j]);
        }
        printf("\n");
	}
}

/*
 * Testing functions only - not useful in a meaningful way
 */


