#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../jsmn.h"

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */

 static char * readJSONFile(){    // 파일 읽어들이는 함수
  char oneline[255];
  char * c = (char *)malloc(6); // 먼저 메모리 할당
  int count=0;
  // char c[1000];
  FILE *fp = fopen("/home/u21000127/jsmn/cars.json","r");	// data.json 읽기 모드로 열기
  while(fgets(oneline,255,fp)!= NULL){  // 읽어들이는 내용 없을 떄까지
    count += strlen(oneline); // 한번 읽어들이는 문자 개수
    c = (char *)realloc(c,count+1); // 누적된 문자개수에 맞는 메모리 할당
    strcat(c,oneline);
  }
  if(feof(fp)!=0)
    puts("복사 완료");
  fclose(fp);

  return c;
 }

 int makecarlist(const char *json, jsmntok_t *t, int tokcount, car_t * list[]){
    int i,count=1;
    char * value1[4];
    char * value2[4];
    char * value3[4];
    char * value4[4];
    for(i=0;i<4;i++){
      value1[i] = (char *)malloc(10);
      value2[i] = (char *)malloc(10);
      value3[i] = (char *)malloc(10);
      value4[i] = (char *)malloc(10);

    }
    for(i=0;i<tokcount;i++){
      if(t[i].size >= 1 && t[i].type == JSMN_OBJECT && t[i].parent != -1){ // 제품 정보담은 객체 추출
        list[count] = (car_t *)malloc(sizeof(car_t));
        int size1 = t[i+2].end-t[i+2].start;
        value1[count-1] =(char *)realloc(value1[count-1],size1+1);
        strcat(list[count]->model,strncpy(value1[count-1],json+t[i+2].start,size1));
        int size2 = t[i+4].end-t[i+4].start;
        value2[count-1] =(char *)realloc(value2[count-1],size2+1);
        strcat(list[count]->maker,strncpy(value2[count-1],json+t[i+4].start,size2));
        int size3 = t[i+8].end-t[i+8].start;
        value3[count-1] =(char *) realloc(value3[count-1],size3+1);
        strcat(list[count]->gastype,strncpy(value3[count-1],json+t[i+8].start,size3));
        int size4 = t[i+6].end-t[i+6].start;
        value4[count-1] = (char *)realloc(value4[count-1],size4+1);
        list[count]->year = atoi(strncpy(value4[count-1],json+t[i+6].start,size4));
        printf("%s\t%s\t%s \n", list[count]->model,list[count]->maker,list[count]->gastype);
        count++; // list배열 번호 증가
      }
    }
    // free(value1);
    // free(value2);
    // free(value3);
    // free(value4);
    return count-1;
 }

 void printcarlist(car_t * list[], int carcount){
   printf("출력 예\n");
   int i;
   printf("번호 모델명\t제조사\t\t제조년도\t연료 타입\n");
   for(i=0;i<carcount;i++){
      printf("%-3d %-10s\t%-10s\t%-10d\t%-10s\n",i+1,list[i+1]->model,list[i+1]->maker,list[i+1]->year,list[i+1]->gastype );
   }

 }

// static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
// 	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
// 			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
// 		return 0;
// 	}
// 	return -1;
// }

int main() {
  char *JSON_STRING = readJSONFile();
	int i;
	int r;
	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));
  car_t * carlist[5];
  // for(i=0;i<5;i++){
  //   carlist[i] = (car_t *)malloc(sizeof(car_t));
  // }
  // carlist[1] = malloc(1000);
  // strcat(carlist[1]->model,"ddd");  // 왜 이건 안됨?
  // printf("%s\n",carlist[1]->model );
  //
  // car_t list[5];
  // strcat(list[1].model,"fff");  // 이건 되는데
  // printf("%s\n",list[1].model );


  int num = makecarlist(JSON_STRING,t,r,carlist);
  printcarlist(carlist,num);
  // free(carlist[1]);
  // free(carlist[2]);
  // free(carlist[3]);
  // free(carlist[4]);



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
	// 	if (jsoneq(JSON_STRING, &t[i], "user") == 0) {
	// 		/* We may use strndup() to fetch string value */
	// 		printf("- User: %.*s\n", t[i+1].end-t[i+1].start,
	// 				JSON_STRING + t[i+1].start);
	// 		i++;
	// 	} else if (jsoneq(JSON_STRING, &t[i], "admin") == 0) {
	// 		/* We may additionally check if the value is either "true" or "false" */
	// 		printf("- Admin: %.*s\n", t[i+1].end-t[i+1].start,
	// 				JSON_STRING + t[i+1].start);
	// 		i++;
	// 	} else if (jsoneq(JSON_STRING, &t[i], "uid") == 0) {
	// 		/* We may want to do strtol() here to get numeric value */
	// 		printf("- UID: %.*s\n", t[i+1].end-t[i+1].start,
	// 				JSON_STRING + t[i+1].start);
	// 		i++;
	// 	} else if (jsoneq(JSON_STRING, &t[i], "groups") == 0) {
	// 		int j;
	// 		printf("- Groups:\n");
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
}
