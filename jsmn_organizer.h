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
#ifndef JSMN_ORGANIZER_H_
#define JSMN_ORGANIZER_H_

#include "printf.h"
#include "jsmn.h"

typedef enum
{
	MAIN_OBJECT 			= 0,
	SUB_OBJECT				= 1 << 0,
//	SUB_OBJECT_INSIDE_ARRAY	= 1 << 1,
}jsmn_object_type_t;



typedef struct
/*
 * SUB_OBJECTs are always Values never Keys
 */
{
	jsmn_object_type_t object_type;		// MAIN_OBJECT or SUB_OBJECT
	int object_boundary_token_index;
	int key_token_for_sub_object_index;
	int key_token_of_array_containing_object_index;

}jsmn_objects_organized_t;


typedef struct
/*
 * Arrays are always Values never Keys
 */
{
	int array_boundary_token_index;
	int key_token_for_array_index;
}jsmn_array_organized_t;

#define EXPECTED_NO_OF_OBJECTS   			10
#define EXPECTED_NO_OF_ARRAYS    			10
#define EXPECTED_NO_OF_INDEXES_TO_NEGLET   	100

typedef struct
{
	int number_of_objects;  				// main object always present so 1 will be minimum.
	jsmn_objects_organized_t objects[EXPECTED_NO_OF_OBJECTS];
	int number_of_arrays;
	jsmn_array_organized_t   arrays[EXPECTED_NO_OF_ARRAYS];
	int number_of_indexes_to_neglet;
	int indexes_to_neglet[EXPECTED_NO_OF_INDEXES_TO_NEGLET];
}jsmn_organized_t;




// Useful function
void jsmn_organize
						(
						jsmntok_t * tokens_received_after_parsing,
						int num_tokens,
						jsmn_organized_t * result
						);

// Useful function
void jsmn_print_organized_key_value_pairs
						(
						jsmn_organized_t * result,
						const char *json,
						const jsmntok_t *tokens,
						int num_tokens
						);






// Testing function
void print_all_key_value_pairs
						(
						const char *json,
						const jsmntok_t *tokens,
						int num_tokens
						);

// Testing function
void print_all_tokens_and_values
						(
						const char *json,
						const jsmntok_t *tokens,
						int num_tokens
						);



#endif /* JSMN_ORGANIZER_H_ */
