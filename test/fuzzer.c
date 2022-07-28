#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "../jsmn.h"

extern "C"
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size){
        char *js = (char *)malloc(size+1);
        if (js == NULL){
                return 0;
        }
        memcpy(js, data, size);
        js[size] = '\0';

        int r;
        jsmn_parser p;
        jsmntok_t tokens[128];
        jsmn_init(&p);
        r = jsmn_parse(&p, js, strlen(js), tokens, 128);


        free(js);
        return 0;
}
