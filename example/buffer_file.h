#ifndef __BUFFER_FILE__
#define __BUFFER_FILE__
#include <stdint.h>
#include <stdbool.h>

typedef struct BUFFILEstr {
  int malloced;
  uint8_t* data;
  size_t len;
  size_t limit;
} *pBUFFILE, **ppBUFFILE, BUFFILE;

uint8_t* buffile_free(pBUFFILE pBuffile, size_t* pBuf_len);
void buffile_init(pBUFFILE pBuffile, int malloced);
pBUFFILE buffile_size(pBUFFILE pBuffile, size_t to_add);
pBUFFILE buffile_write(void* pVoidBuffile, size_t memb_size, size_t n_memb, void* pSource);
uint8_t* buffile_file_to_puint8(char* filename, size_t* pBuf_len, pBUFFILE pBuffile);

/* Set upper limit for buffer size add 100MBi */
#define BUFFILE_UPPER_LIMIT ((size_t)100000000 )

#endif // __BUFFER_FILE__
