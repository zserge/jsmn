#include "jsmn.h"
#include "jsmn_string.h"

static int hex2int(char ch){
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    return -1;
}

const char *jsmn_string(char *buf, const jsmntok_t *tk){

	if (tk->type != JSMN_STRING)
		return NULL;

	if (buf[tk->start-1] == '\0') //already decoded...
		return &buf[tk->start];

	if (buf[tk->start-1] == '!') //already decoded...
		return NULL;

	//decode the string... it is valid utf8... so no worries
	char *dst = &buf[tk->start];
	const char *src = dst;
	const char *end = &buf[tk->end];
	for (;src!=end;src++) {
		if (*src & 0x80) {	//get utf8
			while (*src & 0x80)
				*(dst++) = *(src++);
			continue;
		}

		if (*src == '"')	//there should be only one closing " (that we never scan @ tk->end)
				goto UTF8_ERROR;

		if (*src == '\\') {
			if (*(src+1) == 'u') {	//escape of unicode code point (decode code point to utf8)
				//get a unicode code point
				int uni = 0;
				int i;
				for (i = 0; i < 4; i++) {
					src++;
					if (src == end)
						goto UTF8_ERROR;

					int c = hex2int(*src);
					if (c < 0)
						goto UTF8_ERROR;

					uni <<= 4;
					uni |= c;
				}

				//reencode as utf8 bytes
				if (uni == 0x00) {
					#if JSMN_STRING_EMBEDDED_NULL == 0
					*dst++=uni;
					#elif JSMN_STRING_EMBEDDED_NULL == 1
					*dst++=0xC0;
					*dst++=0x80;
					continue;
					#elif JSMN_STRING_EMBEDDED_NULL == 2
					goto UTF8_ERROR;
					#else
						#error JSMN_STRING_EMBEDDED_NULL must be 0,1, or 2
					#endif
				} else if (uni < 0x80)
					*dst++=uni;
				else if (uni < 0x800) {
					*dst++=0xC0 + uni/64;
					*dst++=0x80 + uni%64;
				} else if (uni < 0xE000) {
					goto UTF8_ERROR;
				} else if (uni < 0x10000) {
					*dst++=0xE0 + uni/4096;
					*dst++=0x80 + uni/64%64;
					*dst++=0x80 + uni%64;
				} else if (uni < 0x110000) {
					*dst++=0xF0 + uni/262144;
					*dst++=0x80 + uni/4096%64;
					*dst++=0x80 + uni/64%64;
					*dst++=0x80 + uni%64;
				} else goto UTF8_ERROR;
				continue;
			}

			switch (*(src+1)) {
			//decode special
			case 'b': *(dst++) = '\b'; break;
			case 'f': *(dst++) = '\f'; break;
			case 'n': *(dst++) = '\n'; break;
			case 'r': *(dst++) = '\r'; break;
			case 't': *(dst++) = '\t'; break;

			//decode next (single byte utf8)
			default: *(dst++) = *(src+1); break;
			}

			src++;
			continue;
		}

		//get ordinary single byte utf8
		*(dst++) = *src;
	}
	*(dst++)='\0';

	buf[tk->start-1] = '\0';
	buf[tk->end] = '\0';
	return &buf[tk->start];

UTF8_ERROR:;
	buf[tk->start-1] = '!';
	return NULL;
}

