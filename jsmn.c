#include "jsmn.h"
#include <stdio.h>


/**
 * Allocates a fresh unused token from the token pull.1
 */
static jsmntok_t *jsmn_alloc_token(jsmn_parser *parser,
		jsmntok_t *tokens, size_t num_tokens) {		// 새로운 토큰 하나 생성
	jsmntok_t *tok;
	if (parser->toknext >= num_tokens) {	// toknext 는 tokens 배열(128개)에서의 위치를 나타냄
		return NULL;
	}
	tok = &tokens[parser->toknext++];	// 새로 토큰 할당하면서 다음 토큰 번호 +1 되게 함
	tok->start = tok->end = -1;		// start,end 는 json 형태에서의 위치, 처음 생성될 때 -1로 초기화
	tok->size = 0;		//  0으로 초기화
#ifdef JSMN_PARENT_LINKS		// 이 변수가 선언되어 있다면
	tok->parent = -1;	// -1이라는 건 없다는 것이나 마찬가지, 배열은 0부터 시작하기 때문
#endif											// if 문 닫기
	return tok;
}

/**
 * Fills token type and boundaries.
 */
static void jsmn_fill_token(jsmntok_t *token, jsmntype_t type,
                            int start, int end) {		// 토큰의 타입,시작 위치,끝위치 결정
	token->type = type;			// 토큰의 type에 argument type을 할당
	token->start = start;		// 토큰의 start에 argument start을 할당
	token->end = end;		// 토큰의 end에 argument end을 할당
	token->size = 0;
}

/**
 * Fills next available token with JSON primitive.
 */
static int jsmn_parse_primitive(jsmn_parser *parser, const char *js,
		size_t len, jsmntok_t *tokens, size_t num_tokens) {
	jsmntok_t *token;
	int start;

	start = parser->pos;

	for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
		switch (js[parser->pos]) {
#ifndef JSMN_STRICT
			/* In strict mode primitive must be followed by "," or "}" or "]" */
			case ':':
#endif
			case '\t' : case '\r' : case '\n' : case ' ' :
			case ','  : case ']'  : case '}' :
				goto found;				// found 로 이동
		}
		if (js[parser->pos] < 32 || js[parser->pos] >= 127) { // 숫자,문자,부호가 아니라면
			parser->pos = start;	// 처음부터 다시 시작
			return JSMN_ERROR_INVAL;	// json string 안에 invalid character 포함
		}
	}
#ifdef JSMN_STRICT	// JSMN_STRICT 가 뭐임?
	/* In strict mode primitive must be followed by a comma/object/array */
	parser->pos = start;
	return JSMN_ERROR_PART;	// packet 의 형식이 잘못됨
#endif

found:
	if (tokens == NULL) {
		parser->pos--;
		return 0;
	}
	token = jsmn_alloc_token(parser, tokens, num_tokens);		// 새로운 토큰 생성
	if (token == NULL) {
		parser->pos = start;
		return JSMN_ERROR_NOMEM;	// 토큰이 충분치 않음
	}
	jsmn_fill_token(token, JSMN_PRIMITIVE, start, parser->pos);
#ifdef JSMN_PARENT_LINKS
	token->parent = parser->toksuper;	// 상위토큰 parent에 저장
#endif
	parser->pos--;
	return 0;
}

/**
 * Fills next token with JSON string.
 */
static int jsmn_parse_string(jsmn_parser *parser, const char *js,
		size_t len, jsmntok_t *tokens, size_t num_tokens) {		// string 형태 ,즉 " 가 있을 때 수행되는 함수
	jsmntok_t *token;

	int start = parser->pos;

	parser->pos++;

	/* Skip starting quote */
	for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
		char c = js[parser->pos];

		/* Quote: end of string */
		if (c == '\"') {		// string 의 끝을 나타내는 " 일 때
			if (tokens == NULL) {
				return 0;
			}
			token = jsmn_alloc_token(parser, tokens, num_tokens);	// 토큰 새로 생성
			if (token == NULL) {	// 토큰 생성된게 없으면 처음부터 다시 실행되게 함
				parser->pos = start;
				return JSMN_ERROR_NOMEM;
			}
			jsmn_fill_token(token, JSMN_STRING, start+1, parser->pos); // string 토큰에 정보 담음
#ifdef JSMN_PARENT_LINKS
			token->parent = parser->toksuper;
#endif
			return 0;
		}

		/* Backslash: Quoted symbol expected */
		if (c == '\\' && parser->pos + 1 < len) {		//  \가 포함된 경우
			int i;
			parser->pos++;
			switch (js[parser->pos]) {
				/* Allowed escaped symbols */
				case '\"': case '/' : case '\\' : case 'b' :
				case 'f' : case 'r' : case 'n'  : case 't' :
					break;
				/* Allows escaped symbol \uXXXX */
				case 'u':
					parser->pos++;
					for(i = 0; i < 4 && parser->pos < len && js[parser->pos] != '\0'; i++) {
						/* If it isn't a hex character we have an error */
						if(!((js[parser->pos] >= 48 && js[parser->pos] <= 57) || /* 0-9 */
									(js[parser->pos] >= 65 && js[parser->pos] <= 70) || /* A-F */
									(js[parser->pos] >= 97 && js[parser->pos] <= 102))) { /* a-f */ // 이 범위 안이 아니라면
							parser->pos = start;
							return JSMN_ERROR_INVAL;
						}
						parser->pos++;
					}
					parser->pos--;
					break;
				/* Unexpected symbol */
				default:
					parser->pos = start;
					return JSMN_ERROR_INVAL;
			}
		}
	}
	parser->pos = start;
	return JSMN_ERROR_PART;
}

/**
 * Parse JSON string and fill tokens.
 */
int jsmn_parse // string 안의 char 하나씩 다 조사함
							(jsmn_parser *parser, // 현재 토큰의 pos + 다음 토큰의 pos + 상위 토큰의 pos
							 const char *js,		// 조사할 대상 string
							 size_t len,				// 조사할 대상 string 의 전체 길이
							 jsmntok_t *tokens,	// string 을 나눌 토큰 배열
		 				 	 unsigned int num_tokens) {		// 토큰의 전체 개수
	int r;
	int i;
	jsmntok_t *token;
	int count = parser->toknext;

	for (; parser->pos < len && js[parser->pos] != '\0'; parser->pos++) {
		char c;
		jsmntype_t type;

		c = js[parser->pos];
		switch (c) {
			case '{': case '[':		// object 나 array 시작할 때
				count++;			// case 통과할 때 마다 +1
				if (tokens == NULL) {	// 토큰 담을 배열 존재하는지 점검
					break;
				}
				token = jsmn_alloc_token(parser, tokens, num_tokens);		// object 나 array 토큰 생성
				if (token == NULL)
					return JSMN_ERROR_NOMEM;
				if (parser->toksuper != -1) {	// 맨처음 { 제외한다는 뜻,즉 상위토큰이 있는 경우를 말함 "repository"(7), "examples"(17)
					tokens[parser->toksuper].size++;
//					printf("sdfsd%d\n",parser->toksuper);
//					printf("kkkkkkk");
#ifdef JSMN_PARENT_LINKS
					token->parent = parser->toksuper;
#endif
				}
				token->type = (c == '{' ? JSMN_OBJECT : JSMN_ARRAY);
				token->start = parser->pos;			// 토큰의 시작 위치 설정
				parser->toksuper = parser->toknext - 1;	// toksuper 값에 현재 토큰의 번호 대입
																								// toksuper 값이 다음
				break;
			case '}': case ']':			// object 나 array 끝날 때
				if (tokens == NULL)
					break;
				type = (c == '}' ? JSMN_OBJECT : JSMN_ARRAY);
#ifdef JSMN_PARENT_LINKS
				if (parser->toknext < 1) {
					return JSMN_ERROR_INVAL;
				}
				token = &tokens[parser->toknext - 1];
				for (;;) {
					if (token->start != -1 && token->end == -1) {	// obj 나 arr 열려있다면
						if (token->type != type) {
							return JSMN_ERROR_INVAL;
						}
						token->end = parser->pos + 1;
						parser->toksuper = token->parent;
						break;
					}
					if (token->parent == -1) {
						if(token->type != type || parser->toksuper == -1) {
							return JSMN_ERROR_INVAL;
						}
						break;
					}
					token = &tokens[token->parent];
				}
#else
				for (i = parser->toknext - 1; i >= 0; i--) {
					token = &tokens[i];
					if (token->start != -1 && token->end == -1) {		// end가 -1 이라는 건 아직 object 나 arrary가 닫히지 않았다는 뜻.
						if (token->type != type) {										//(1) 열려 있는 arr,obj 토큰 중 가장 가까운 토큰, 즉 자신(}) 과 한 쌍인 { 의 토큰
							return JSMN_ERROR_INVAL;
						}
						parser->toksuper = -1;	// 이건 왜 있는거??
						token->end = parser->pos + 1;	// 토큰의 끝 위치 설정, 더 이상 -1 이 아니게 됨(닫힘을 의미)
						break;
					}
				}
				/* Error if unmatched closing bracket */
				if (i == -1) return JSMN_ERROR_INVAL;
				for (; i >= 0; i--) {
					token = &tokens[i];													// 아직 닫히지 않은 arr, obj 토큰 중 가장 가까운 토큰,
					if (token->start != -1 && token->end == -1) {		//  즉 자신(})이 방금 (1)에서 닫은 obj 토큰보다 한 단계 더 큰 obj 토큰의
						parser->toksuper = i;													// 번호를 toksuper에 대입, but (1)에서 가장 큰 obj까지 모두 닫았으면 i =0 이 됨.
						break;
					}
				}
#endif
				break;
			case '\"':
				r = jsmn_parse_string(parser, js, len, tokens, num_tokens);
				if (r < 0) return r;	// r<0 일 때, 오류 나타냄
				count++;	// case 통과할 때마다 토큰개수에 +1 해줌
				if (parser->toksuper != -1 && tokens != NULL)
					tokens[parser->toksuper].size++;	// ---(1)
				break;
			case '\t' : case '\r' : case '\n' : case ' ':
				break;
			case ':':
				parser->toksuper = parser->toknext - 1;		// : 전의 string 토큰번호를 toksuper에 대입
				break;																		// : 이후에 뭔가 왔을 때 : 전의 토큰.size가 +1 되도록
			case ',':			// , 이후에 string 오면 닫히지 않은 ,가장 근처의 array 나 object token.size가 +1 되도록 설정한 것임.---(1)
				if (tokens != NULL && parser->toksuper != -1 &&
						tokens[parser->toksuper].type != JSMN_ARRAY &&
						tokens[parser->toksuper].type != JSMN_OBJECT) {
#ifdef JSMN_PARENT_LINKS
					parser->toksuper = tokens[parser->toksuper].parent;	// parent 로 더 간단하게 가능.    , 가 오면 키 값의 부모토큰번호를 toksuper에 대입
#else
					for (i = parser->toknext - 1; i >= 0; i--) {
						if (tokens[i].type == JSMN_ARRAY || tokens[i].type == JSMN_OBJECT) {
							if (tokens[i].start != -1 && tokens[i].end == -1) { // 닫히지 않은, 가장 근처에 있는 array 나 object token 번호를
								parser->toksuper = i;														// toksuper 에 대입
								break;
							}
						}
					}
#endif
				}
				break;
#ifdef JSMN_STRICT		// 이 변수가 정의되어 있을 떄
			/* In strict mode primitives are: numbers and booleans */
			printf("여기는 안지나감\n");
			case '-': case '0': case '1' : case '2': case '3' : case '4':
			case '5': case '6': case '7' : case '8': case '9':
			case 't': case 'f': case 'n' :
				/* And they must not be keys of the object */
				if (tokens != NULL && parser->toksuper != -1) {
					jsmntok_t *t = &tokens[parser->toksuper];
					if (t->type == JSMN_OBJECT ||
							(t->type == JSMN_STRING && t->size != 0)) {
						return JSMN_ERROR_INVAL;
					}
				}
#else
			/* In non-strict mode every unquoted value is a primitive */
			default:
#endif
				r = jsmn_parse_primitive(parser, js, len, tokens, num_tokens);
				if (r < 0) return r;
				count++;
				if (parser->toksuper != -1 && tokens != NULL)
					tokens[parser->toksuper].size++;
	//			printf("지나감?\n");
				break;

#ifdef JSMN_STRICT
			/* Unexpected char in strict mode */
			default:
				return JSMN_ERROR_INVAL;
#endif
		}
	}

	if (tokens != NULL) {
		for (i = parser->toknext - 1; i >= 0; i--) {
			/* Unmatched opened object or array */
			if (tokens[i].start != -1 && tokens[i].end == -1) {
				return JSMN_ERROR_PART;
			}
		}
	}
//	printf("count:%d\n",count );
	return count;
}

/**
 * Creates a new parser based over a given  buffer with an array of tokens
 * available.
 */
void jsmn_init(jsmn_parser *parser) {		// parser 초기화
	parser->pos = 0;
	parser->toknext = 0;
	parser->toksuper = -1;
}
