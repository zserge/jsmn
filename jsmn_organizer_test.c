/*
Test code for jsmn_organize functions.
Put these inside a function / or inside main() to run this
*/

/*
 * Copy Start 
 */

#include "jsmn.h"
#include "jsmn_organize.h"


#define   JSON_STRING	"{\"name\": \"John\",\"age\": 30,\"isStudent\": false,\"marks\": [\"address\": {\"city\": \"New York\",\"country\": \"USA\"}, 90, 85, 95],\"address\": {\"city\": \"New York\",\"country\": \"USA\"},\"languages\": [\"English\", \"Spanish\"]}"



const char *json = JSON_STRING;
const size_t json_len = strlen(json);

// Define an array of tokens with a size suitable for the JSON string
jsmntok_t tokens[128]; // Adjust the size as needed

// Initialize the parser
jsmn_parser parser;
jsmn_init(&parser);

// Parse the JSON string
int num_tokens = jsmn_parse(&parser, json, json_len, tokens, sizeof(tokens) / sizeof(tokens[0]));

// Check for parsing errors
if (num_tokens < 0) {
  printf("JSON parsing failed with error code %d\n", num_tokens);
  return 1;
}

// Iterate over tokens and print their types and boundaries
printf("Parsed JSON with %d tokens:\n", num_tokens);
for (int i = 0; i < num_tokens; i++) {
  printf("Token %d: Type=%d, Start=%d, End=%d\n", i, tokens[i].type, tokens[i].start, tokens[i].end);
}



jsmn_organized_t result = {0};

jsmn_organize
    (
      tokens,
      num_tokens,
      &result
    );
printf("\n\n\n");
print_all_tokens_and_value(json,  tokens,  num_tokens);
printf("\n\n\n");
jsmn_print_organized_key_value_pairs(&result, json, tokens,  num_tokens);
printf("\n\n\n");

/*
 * Copy End - Paste Inside main() and run
 */
