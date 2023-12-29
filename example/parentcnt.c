#include "jsmn_tools.h"
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


static int read_file(const char * filename, char ** pbuffer);

/**
 * Main function showcasing JSON parsing using JSMN with defined JSMN_PARENT_CNT.
 *
 * This function reads a sample JSON file, utilizes the JSMN library with a
 * specifically defined JSMN_PARENT_CNT during compilation, and extracts information
 * about operations to execute. The JSON structure includes data such as serial numbers,
 * indices, and states for outlets. The provided example JSON is as follows:
 *
 *        {
 *          "version": "0.1",
 *          "endpoint": "outlets",
 *          "cmd": "set",
 *          "switches": [
 *            {
 *              "serial": "12345",
 *              "index": [0, 1, 2, 3],
 *              "state": ["open", "close", "open", "close"]
 *            },
 *            {
 *              "serial": "67890",
 *              "index": [0, 1, 2, 3],
 *              "state": ["open", "open", "open", "close"]
 *            }
 *          ],
 *          "test": {
 *            "test2": {
 *              "key": "value"
 *            }
 *          }
 *        }
 *
 * The expected output would be:
 *
 *        serial 12345: index 0 to open
 *        serial 12345: index 1 to close
 *        serial 12345: index 2 to open
 *        serial 12345: index 3 to close
 *        serial 67890: index 0 to open
 *        serial 67890: index 1 to open
 *        serial 67890: index 2 to open
 *        serial 67890: index 3 to close
 */
int main()
{
    jsmn_parser p;
    jsmntok_t * t ;
    jsmn_key_val_t out[] = {  {"endpoint", 0}, {"cmd", 0} , {"switches", 0} };
    char * buffer ;
    int len = read_file ("parentcnt.json", &buffer) ;

    if (len <= 0) {
        return 1;
    }

    printf("parsing json...\r\n");
    jsmn_init(&p);
    int r = jsmn_parse(&p, buffer, len, 0, 0); // content is the char array holding the json content

    printf("%d objects found!\r\n", r);
    if (r <= 0) {
        printf("Error invalid json\r\n");
        return 1;
    }

    t = malloc (r * sizeof(jsmntok_t)) ;
    if (!t) {
        printf("Error out of memory\r\n");
        return 1;
    }

    jsmn_init(&p);
    r = jsmn_parse (&p, buffer, len, t, r) ;
    jsmn_object_enum (buffer, t, r, out, 3) ;
    printf("\r\n\r\n");

    jsmntok_t * switches = out[2].value;
    if (switches) {
    	/* A "switches" array is present.
    	 * Assign "next" to the first object in the array.
    	 */
        jsmntok_t * next = switches + 1 ;

        /* Enumerate over all the objects in the array. */
        do  {
            /* out3 is declared here to clear the `values` of out3 to NULL.
             * jsmn_object_enum only assigns a value if it is NULL.
             */
            jsmn_key_val_t out3[] = {  {"serial", NULL}, {"index", NULL} ,
                                       {"state", NULL} } ;
            jsmn_object_enum (buffer, next, next->size, out3, 3) ;
            jsmntok_t * serial = out3[0].value ;

            if (serial) {
            	/* The serial key is present so it is a valid object. */
                char szserial[12] ;
                /* Extract the two arrays we will use to switch the outlets. */
                jsmntok_t * next_index = out3[1].value + 1 ;
                jsmntok_t * next_state = out3[2].value + 1 ;

                jsmn_copy_str (buffer, serial, szserial, 12) ;

                while (next_index && next_state)  {
                	/* Enumerate over all the indices in the arrays and switch the outlets. */
                    char szindex[8] ;
                    char szstate[8] ;

                    jsmn_copy_str (buffer, next_index, szindex, 8) ;
                    jsmn_copy_str (buffer, next_state, szstate, 8) ;

                    /* We don't really switch, just print the result. */
                    printf("serial %s: index %s to %s\r\n",
                                     szserial, szindex, szstate) ;

                    next_index = jsmn_get_next (out3[1].value, next_index) ;
                    next_state = jsmn_get_next (out3[2].value, next_state) ;

                }

            }

        } while ((next = jsmn_get_next (switches, next))) ;

    }

    /* Don't forget to free the allocated memory when you're done */
    free(buffer);

    return 0;
}


int read_file(const char * filename, char ** pbuffer)
{
    FILE *file;
    char *buffer;
    long len;

    /* Open the file in binary read mode */
    file = fopen(filename, "rb");

    if (file == NULL) {
        printf("Error opening the file");
        return 0;
    }

    /* Get the file length */
    fseek(file, 0, SEEK_END);
    len = ftell(file);
    rewind(file);

    /* Allocate memory for the buffer */
    buffer = (char *)malloc(len);

    if (buffer == NULL) {
        printf("Error allocating memory");
        return 0;
    }

    /* Read the entire file into the buffer  */
    len = fread(buffer, 1, len, file);
    /* Close the file */
    fclose(file);

    *pbuffer = buffer;

    return len;
}
