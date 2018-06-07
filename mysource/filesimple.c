#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../jsmn.h"

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */
 static char * readJSONFile(){
 //	char *JSON_STRING="{\"u\": \"dd\"}";
 	char oneline[255];
	char * c = (char *)malloc(600);
	FILE *fp = fopen("/home/u21000127/jsmn/data.json","r");	// data.json 읽기 모드로 열기
  while(fgets(oneline,255,fp)!= NULL){
    strcat(c,oneline);
  }
  if(feof(fp)!=0)
  //  puts("복사 완료");
  fclose(fp);

  return c;
 }

void jsonNameList(char *jsonstr, jsmntok_t *t, int tokcount, int *nameTokIndex ){
    int i,count=1;
//    nameTokIndex = (int *)malloc(sizeof(int)*100);
    for(i=0;i<tokcount;i++){ // 100 개까지만
      if(t[i].size == 1 && t[i].type == JSMN_STRING){
        nameTokIndex[count] = i;
        count++;
      }
    }
    nameTokIndex[0] = count - 1;
}

void printNameList(char *jsonstr, jsmntok_t *t, int *nameTokIndex){
    printf("**** Name List ****\n");
    int i;
    for(i=0;i<nameTokIndex[0];i++){
      printf("[NAME %d]  %.*s\n", i+1, t[nameTokIndex[i+1]].end-t[nameTokIndex[i+1]].start,
      jsonstr+t[nameTokIndex[i+1]].start );
    }

}

char * noValue(char *jsonstr, jsmntok_t *t, int *nameTokIndex, int num){
    int size = t[nameTokIndex[num]+1].end-t[nameTokIndex[num]+1].start;
    char * value = (char *)malloc(size);
    strncpy(value, jsonstr+t[nameTokIndex[num]+1].start, size);
    return value;
}

void selectNameList(char *jsonstr, jsmntok_t *t, int *nameTokIndex){

    int num;
    char *value;
    while(1){
      printf("Select Name's no (exit:0) >>");
      scanf(" %d", &num);
      if(num == 0) break;
      value = noValue(jsonstr,t,nameTokIndex,num);
      printf("[NAME %d]  %.*s\n %s\n\n", num, t[nameTokIndex[num]].end-t[nameTokIndex[num]].start,
      jsonstr+t[nameTokIndex[num]].start, value );

    }

}
// void jsonNameList(char *jsonstr, jsmntok_t *t, int tokcount){
//     printf("**** Name List ****\n");
//     int i,count=1;
//
//     for(i=0;i<tokcount;i++){
//       if(t[i].size == 1 ){
//         printf("[NAME %d]  %.*s\n",count,t[i].end-t[i].start,jsonstr+t[i].start );
//         count++;
//       }
//     }
// }
// static const char *JSON_STRING =
// 	"{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000,\n  "
// 	"\"groups\": [\"users\", \"wheel\", \"audio\", \"video\"]}";

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {  // 일치하는지 확인
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

int main() {
  char *JSON_STRING = readJSONFile();
//  printf("%s\n", JSON_STRING );
	int i;
	int r;
	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));
  // parse해야 t 배열에 토큰들 담김

  int arr[100];
  jsonNameList(JSON_STRING,t,100,arr);
//  printNameList(JSON_STRING,t,arr);
  selectNameList(JSON_STRING,t,arr);
	// if (r < 0) {
	// 	printf("Failed to parse JSON: %d\n", r);
	// 	return 1;
	// }
  //
	// /* Assume the top-level element is an object */
	// if (r < 1 || t[0].type != JSMN_OBJECT) {
	// 	printf("Object expected\n");
	// 	return 1;
	// }
  //
	// /* Loop over all keys of the root object */
	// for (i = 1; i < r; i++) {
	// 	if (jsoneq(JSON_STRING, &t[i], "name") == 0) {
	// 		/* We may use strndup() to fetch string value */
	// 		printf("- name: %.*s\n", t[i+1].end-t[i+1].start,
	// 				JSON_STRING + t[i+1].start);
	// 		i++;
	// 	} else if (jsoneq(JSON_STRING, &t[i], "keywords") == 0) {
	// 		/* We may additionally check if the value is either "true" or "false" */
	// 		printf("- keywords: %.*s\n", t[i+1].end-t[i+1].start,
	// 				JSON_STRING + t[i+1].start);
	// 		i++;
	// 	} else if (jsoneq(JSON_STRING, &t[i], "description") == 0) {
	// 		/* We may want to do strtol() here to get numeric value */
	// 		printf("- UID: %.*s\n", t[i+1].end-t[i+1].start,
	// 				JSON_STRING + t[i+1].start);
	// 		i++;
	// 	} else if (jsoneq(JSON_STRING, &t[i], "examples") == 0) {
	// 		int j;
	// 		printf("- examples:\n");
	// 		if (t[i+1].type != JSMN_ARRAY) {
	// 			continue; /* We expect groups to be an array of strings */
	// 		}
	// 		for (j = 0; j < t[i+1].size; j++) {
	// 			jsmntok_t *g = &t[i+j+2];
	// 			printf("  * %.*s\n", g->end - g->start, JSON_STRING + g->start);
	// 		}
	// 		i += t[i+1].size + 1;
	// 	} else {
	// 		printf("Unexpected key: %.*s\n", t[i].end-t[i].start,
	// 				JSON_STRING + t[i].start);
	// 	}
	// }
	// return EXIT_SUCCESS;
  free(JSON_STRING);
}
