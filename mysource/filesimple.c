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
	char * c = (char *)malloc(6);
  int count=0;
  // char c[1000];
	FILE *fp = fopen("/home/u21000127/jsmn/data4.json","r");	// data.json 읽기 모드로 열기
  while(fgets(oneline,255,fp)!= NULL){
    count += strlen(oneline);
    c = (char *)realloc(c,count+1);
    strcat(c,oneline);
  }
  if(feof(fp)!=0)
  //  puts("복사 완료");
  fclose(fp);

  return c;
 }

 static char * readSelectJSONFile(){
 //	char *JSON_STRING="{\"u\": \"dd\"}";
   char fileName[50],address[70]="/home/u21000127/jsmn/",end[10]=".json";
   printf("원하는 파일명 입력: ");
   scanf(" %s", fileName);
   strcat(address,fileName);
   strcat(address,end);
	FILE *fp = fopen(address,"r");	// data.json 읽기 모드로 열기
  if(fp == NULL){
   printf("%s.json 파일이 존재하지 않습니다\n",fileName);
   exit(0);
 }
  char oneline[255];
  char * c = (char *)malloc(60);
  int count=0;

  while(fgets(oneline,255,fp)!= NULL){
    count += strlen(oneline);
    c = (char *)realloc(c,count+1);
    strcat(c,oneline);
  }
  if(feof(fp)!=0)
  //  puts("복사 완료");
  fclose(fp);

  return c;
 }

 void jsonNameList(char *jsonstr, jsmntok_t *t, int tokcount){  // example 4
     printf("**** Name List ****\n");
     int i,count=1;

     for(i=0;i<tokcount;i++){
       if(t[i].size == 1 ){
         printf("[NAME %d]  %.*s\n",count,t[i].end-t[i].start,jsonstr+t[i].start );
         count++;
       }
     }
 }
 // static const char *JSON_STRING =
 // 	"{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000,\n  "
 // 	"\"groups\": [\"users\", \"wheel\", \"audio\", \"video\"]}";

void jsonNameList(char *jsonstr, jsmntok_t *t, int tokcount, int *nameTokIndex ){
    int i,count=1;
//    nameTokIndex = (int *)malloc(sizeof(int)*100);
    for(i=0;i<tokcount;i++){
      if(t[i].size == 1 && t[i].type == JSMN_STRING){ // key(name)값일 때
        nameTokIndex[count] = i;
        count++;
      }
    }
    nameTokIndex[0] = count - 1;  // key(Name) 값이 되는 토큰 개수 저장
}

void jsonNameList(char *jsonstr, jsmntok_t *t, int tokcount, int *nameTokenIndex ){ // example 5
    int i,count=1;
//    nameTokIndex = (int *)malloc(sizeof(int)*100);
    for(i=0;i<tokcount;i++){
      if(t[i].size == 1 && t[i].type == JSMN_STRING){ // key(name)값일 때
        nameTokIndex[count] = i;
        count++;
      }
    }
    nameTokIndex[0] = count - 1;  // key(Name) 값이 되는 토큰 개수 저장
}

void jsonBigNameList(char *jsonstr, jsmntok_t *t, int tokcount, int *nameTokIndex ){  // parent = 0 인 경우만
    int i,count=1;
//    nameTokIndex = (int *)malloc(sizeof(int)*100);
    for(i=0;i<tokcount;i++){
      if(t[i].size == 1 && t[i].type == JSMN_STRING && t[t[i].parent].parent == -1){ // '전체 객체'에 속한 key(name)값일 때
        nameTokIndex[count] = i;
        count++;
      }
    }
    nameTokIndex[0] = count - 1;  // key(Name) 값이 되는 토큰 개수 저장
}

void jsonSmallNameList(char *jsonstr, jsmntok_t *t, int tokcount, int *nameTokIndex ){  // example 12
    int i,count=1;
//    nameTokIndex = (int *)malloc(sizeof(int)*100);
    for(i=0;i<tokcount;i++){
      if(t[i].size == 1 && t[i].type == JSMN_STRING && t[t[i].parent].parent != -1){ // '전체 객체'에 속한 key(name)값이 아닐 때
        nameTokIndex[count] = i;
        count++;
      }
    }
    nameTokIndex[0] = count - 1;  // key(Name) 값이 되는 토큰 개수 저장
}

void printNameList(char *jsonstr, jsmntok_t *t, int *nameTokIndex){
    printf("**** Name List ****\n");
    int i;
    for(i=0;i<nameTokIndex[0];i++){   // key 값이 되는 토큰 개수만큼 반복
      printf("[NAME %d]  %.*s\n", i+1, t[nameTokIndex[i+1]].end-t[nameTokIndex[i+1]].start,
      jsonstr+t[nameTokIndex[i+1]].start ); // jsonstr = jsonstr[0] 의 주소값 이므로
    }
    printf("\n\n");
}

char * noValue(char *jsonstr, jsmntok_t *t, int *nameTokIndex, int num){  // 해당 번호 name의 value 값 리턴
    int size = t[nameTokIndex[num]+1].end-t[nameTokIndex[num]+1].start; // +1은 토큰 배열에서 name 다음에 value 이므로 한거임.
    char * value = (char *)malloc(size);
    strncpy(value, jsonstr+t[nameTokIndex[num]+1].start, size);
    return value;
}

void selectNameList(char *jsonstr, jsmntok_t *t, int *nameTokIndex){  // 해당 번호 name의 value값 출력

    int num;
    char *value;
    while(1){
      printf("Select Name's no (exit:0) >>");
      scanf(" %d", &num);
      if(num == 0) break;
      value = noValue(jsonstr,t,nameTokIndex,num);  // num 에 해당하는 name의 value 값 반환
      printf("[NAME %d]  %.*s\n %s\n\n", num, t[nameTokIndex[num]].end-t[nameTokIndex[num]].start,
      jsonstr+t[nameTokIndex[num]].start, value );
    }
}

void printFirstValue(char *jsonstr, jsmntok_t *t, int tokcount, int *objTokIndex){  // '전체 객체' 의 첫번쨰 데이터 value만 출력
    printf("**** Object List ****\n");
    int i,count=0;
    for(i=0;i<tokcount;i++){
  //    printf("번호%d %.*s\n",i,t[i].end-t[i].start,jsonstr+t[i].start );
      if( t[i].type == JSMN_OBJECT && t[i].size >=1 && (t[i-1].size == 0 || i == 0) && t[i].parent == -1 ){  // t[i]가 '전체 객체'일 떄의 조건
          count++;
          printf("[NAME %d] %.*s\n",count,t[i+2].end-t[i+2].start,jsonstr+t[i+2].start );
  //        printf("%d\n",i);
          objTokIndex[count] = i; // '전체 객체'의 토큰 번호를 배열에 저장
      }
    }
}

void printFirstValue2(char *jsonstr, jsmntok_t *t, int tokcount, int *objTokIndex){  // '전체 객체'에 속한 객체의 첫번쨰 데이터 value만 출력
    printf("**** Object List ****\n");
    int i,count=0;
    for(i=0;i<tokcount;i++){
  //    printf("번호%d %.*s\n",i,t[i].end-t[i].start,jsonstr+t[i].start );
      if( t[i].type == JSMN_OBJECT && t[i].size >=1 && (t[i-1].size == 0 || t[i].parent == 0)){  // t[음수].size,start 등은 0으로 되어있음
            count++;
          printf("머지:%d\n",t[-2].start );
          printf("[NAME %d] %.*s\n",count,t[i+2].end-t[i+2].start,jsonstr+t[i+2].start );
  //        printf("%d\n",i);
          objTokIndex[count] = i; // '전체 객체'의 토큰 번호를 배열에 저장
      }
    }
}

void printFirstValue3(char *jsonstr, jsmntok_t *t, int tokcount, int *objTokIndex){  // '전체 객체'에 속한 객체의 첫번쨰 데이터 value만 출력
    printf("**** Object List ****\n");
    int i,count=0;
    for(i=0;i<tokcount;i++){
  //    printf("번호%d %.*s\n",i,t[i].end-t[i].start,jsonstr+t[i].start );
      if( t[i].type == JSMN_OBJECT && t[i].size >=1 && t[i].start != 0){  // 첫번쨰 가장 큰 객체는 제외
            count++;
          printf("[NAME %d] %.*s\n",count,t[i+2].end-t[i+2].start,jsonstr+t[i+2].start );
  //        printf("%d\n",i);
          objTokIndex[count] = i; // '전체 객체'의 토큰 번호를 배열에 저장
      }
    }
}

void printObjectAll(char *jsonstr, jsmntok_t *t, int tokcount,int *nameTokIndex, int *objTokIndex){ // 해당 번호 '전체 객체'의 name,value 출력하는 함수
    int i,num;
    char *value;
    while(1){
      int count=0;
      printf("원하는 번호 입력(Exit:0) :");
      scanf(" %d", &num);
      if(num == 0) break;
      for(i=0;i<nameTokIndex[0];i++){ // 토큰 중에 name 에 해당하는 토큰 개수
        if(t[nameTokIndex[i+1]].start > t[objTokIndex[num]].start &&
        t[nameTokIndex[i+1]].end < t[objTokIndex[num]].end ){ // 해당하는 번호의 object의 start보다 name의 start 가 크고
                                                            // 해당하는 번호의 object의 end보다 name의 end 가 작은 name 들 나타냄
          count++;
          value = noValue(jsonstr,t,nameTokIndex,i+1);  // num 에 해당하는 name의 value 값 반환
          if(count == 1)  // 첫번째 name 만 다르게 출력
            printf("%.*s : %s\n", t[nameTokIndex[i+1]].end-t[nameTokIndex[i+1]].start,
            jsonstr+t[nameTokIndex[i+1]].start, value );
          else
            printf("\t[%.*s]  %s\n", t[nameTokIndex[i+1]].end-t[nameTokIndex[i+1]].start,
            jsonstr+t[nameTokIndex[i+1]].start, value );
        }
      }
    }
}

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {  // 일치하는지 확인
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

int main() {
  char *JSON_STRING = readJSONFile();
 printf("%s\n", JSON_STRING );
	int i;
	int r;
	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));
  // parse해야 t 배열에 토큰들 담김

  int arr[100],objarr[50];
  // jsonNameList(JSON_STRING,t,r,arr);  // r 이 토큰 총 개수, arr 배열에 각 name의 토큰번호 저장
  // jsonBigNameList(JSON_STRING,t,r,arr);
  // jsonSmallNameList(JSON_STRING,t,r,arr);
  // printNameList(JSON_STRING,t,arr);
  // selectNameList(JSON_STRING,t,arr);
  // printFirstValue(JSON_STRING,t,r,objarr);
  // printFirstValue3(JSON_STRING,t,r,objarr);
  // printObjectAll(JSON_STRING,t,r,arr,objarr);
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
