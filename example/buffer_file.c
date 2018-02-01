#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer_file.h"


/***********************************************************************
 * Free pBuffile structure plus  data
 * - *pBuf_len, IFF p_Buf_len is not null, will contain length of data
 * - If pBuf_len is null (zero) just discard the data and free the pointer
 */
uint8_t* buffile_free(pBUFFILE pBuffile, size_t *pBuf_len) {
uint8_t* pRtn_data = 0;

  /* Assume failure:  return zero length and zero pointer */
  if (pBuf_len) *pBuf_len = 0;
  if (!pBuffile) return pRtn_data;

  if (!pBuf_len && pBuffile->data) {
    /* Free buffer if data length is not requested */
    free(pBuffile->data);
  } else if (pBuf_len) {
    /* Save data and pointer if data length is requested */
    pRtn_data = pBuffile->data;
    *pBuf_len = pBuffile->len;
  }
  /* Free the BUFFILE structure */
  if (pBuffile->malloced) { free(pBuffile); }
  return pRtn_data;
}


/***********************************************************************
 * Initialize a BUFFILE, pointed to by pBuffile
 * - The caller sets malloced to non-zero if pBuffile is from a
 *   malloc or realloc or calloc call.
 */
void buffile_init(pBUFFILE pBuffile, int malloced) {
  if (!pBuffile) return;
  pBuffile->malloced = malloced;
  pBuffile->data = 0;
  pBuffile->len = 0;
  pBuffile->limit = 0;
  return;
}


/***********************************************************************
 * Add [to_add] chars to BUFFILE pointed to by pBuffile
 * - Return pBuffile
 * - Allocate BUFFILE and data if pBuffile is null
 */
pBUFFILE buffile_size(pBUFFILE pBuffile, size_t to_add) {
size_t new_limit;

  /* Allocate BUFFILE and data if pBuffile is null */
  if (!pBuffile) {
    pBuffile = malloc(sizeof(*pBuffile));
    if (!pBuffile) return pBuffile;
    buffile_init(pBuffile, 1);
  }

  /* Calculate new limit, allow one extra char for null terminator */
  new_limit = pBuffile->len + to_add + 1;

  if (new_limit > BUFFILE_UPPER_LIMIT) {
    fprintf(stderr
           , "buffile_size():  new limit (%lu) exceeds maximum limit (%lu)\n"
           , (unsigned long) new_limit
           , (unsigned long) BUFFILE_UPPER_LIMIT
           );
  }

  /* Re-allocate data if limit will increase */
  if (new_limit > pBuffile->limit) {
  uint8_t* new_data = new_limit > BUFFILE_UPPER_LIMIT ? 0 : realloc(pBuffile->data, new_limit);
    if (!new_data) {
      /* Free BUFFILE structure and allocated data if realloc failed */
      buffile_free(pBuffile, 0);
      return 0;
    }
    /* Update BUFFILE data pointer and limit */
    pBuffile->data = new_data;
    pBuffile->limit = new_limit;
    /* Put null terminator at limit */
    new_data[pBuffile->limit-1] = '\0';
  }
  return pBuffile;
}


/***********************************************************************
 * Concatenate buffer data (read from file) to pBuffile->data buffer
 * - Return pointer to BUFFILE structure
 * - Return null on failure
 */
pBUFFILE buffile_write(void* pVoidBuffile, size_t memb_size, size_t n_memb, void* pSource) {
pBUFFILE pBuffile = (pBUFFILE) pVoidBuffile;
size_t to_add = memb_size * n_memb;

  /* Return BUFFILE pointer pFuffiles if source pointer is null, or if
   * length of buffer data at pSource is non-positive
   */
  if (!pSource) return pBuffile;
  if (to_add < 1) return pBuffile;

  /* Increase BUFFILE data allocation; return null on failure */
  if (!(pBuffile = buffile_size(pBuffile, to_add))) return 0;

  /* Copy data from pSource to BUFFILE buffer pointer,
   * update BUFFILE length
   */
  memcpy(pBuffile->data + pBuffile->len, pSource, to_add);
  pBuffile->len += to_add;

  /* Return BUFFILE pointer */
  return pBuffile;
}


/***********************************************************************
 * Read from file named filename into data buffer, return pointer to buffer
 * and set *pBuf_len to buffer length
 * ***N.B. If calling with a non-zero pBuffile pointer, call
 *           buffile_init(pBuffile, malloced)
 *         first
 */
uint8_t* buffile_file_to_puint8(char* filename, size_t* pBuf_len, pBUFFILE pBuffile) {
FILE* pFile = filename ? (strcmp(filename,"-") ? fopen(filename,"rb") : stdin) : 0;
char read_buffer[BUFSIZ];
size_t n_read;

  if (!pFile) return 0;

  while (pBuf_len) {

    /* Read next chunk; check for error */
    n_read = fread(read_buffer, 1, sizeof(read_buffer), pFile);
    if (ferror(pFile)) { pBuf_len = 0; break; }

    /* Check for [no more data in file && EOF] */
    if (!n_read && feof(pFile)) { break; }

    /* Append chunk to pBuffile data; break on failure */
    if (!(pBuffile = buffile_write(pBuffile, 1, n_read, read_buffer))) { pBuf_len = 0; break; };
  }

  if (pFile != stdin) fclose(pFile);

  return buffile_free(pBuffile, pBuf_len);
}
/***********************************************************************
 * End of buffer_file library
 **********************************************************************/


#ifdef DO_MAIN
/***********************************************************************
 * Test code
 * - Read file into buffer
 * - Compare buffer contents and length to file
 *
 * Build:  gcc -DDO_MAIN -o test_buffer_file buffer_file.c
 *
 * Run (BASH):  ./test_buffer_file && echo success || echo failure
 */
int
main(int argc, char** argv) {
char* filename = argc > 1 ? argv[1] : "buffer_file.h";
size_t buf_len = 0;
char oneline[1024];
uint8_t* pData = buffile_file_to_puint8(filename, &buf_len, 0);
uint8_t* pData2;
uint8_t* buf_len2;
pBUFFILE localBuffile;
FILE* fIn;
size_t iOffset;

  buffile_init(&localBuffile, 0);
  pData2 = buffile_file_to_puint8(filename, &buf_len2, &localBuffile);

# define RTN \
  if (pData) { free(pData); } \
  if (pData2) { free(pData2); } \
  if (fIn) { fclose(fIn); } \
  return

  /* Open file */
  fIn = fopen(filename,"rb");
  if (!fIn) { RTN -1; }

  /* Fail if buffered data do not exist ... */
  if (!pData || !buf_len || !pData2 || !buf_len2) {
    /* ... unless file is empty */
    if (EOF == fgetc(fIn)) { RTN 0; }
    RTN 1;
  }

  /* Fail if two methods give different lengths */
  if (buf_len != buf_len2) {
      fprintf(stderr,"Per-method length mismatch [%ud != %ud]\n"
             , buf_len, buf_len2
             );
  }

  /* Loop over characters in buffered data, reading characters in parallel
   * from the file, and compare them
   */
  for (iOffset=0; iOffset < buf_len; ++iOffset) {
  int next_char;
    if (EOF == (next_char=fgetc(fIn))) {
      /* If fgetc returns EOF, the file ended early */
      fprintf(stderr,"Early EOF\n");
      RTN 1;
    }
    if (next_char != pData[iOffset] || next_char != pData2[iOffset]) {
      /* If fgets returns a character different than what is in the buffer,
       * then there is a mismatch
       */
      fprintf(stderr,"Mismatch at offset %ld [%d != %ud || %d != %ud]\n"
             , (long)iOffset
             , next_char, pData[iOffset]
             , next_char, pData2[iOffset]
             );
      RTN 2;;
    }
  }
  /* If the file matched the buffer through buf_len fgetc calls, but the
   * next value returned by fgetc is not EOF, then the file has more
   * characters than the buffer
   */
  if (iOffset == buf_len) {
    if (EOF != fgetc(fIn)) { fprintf(stderr, "Late EOF\n"); RTN 3; }
  } else {
  }
  /* Success:  file and buffer match in both content and length */
  RTN 0;
}
#endif // DO_MAIN
