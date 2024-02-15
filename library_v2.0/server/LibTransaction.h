#ifndef _LIB_TRANSACTION_
#define _LIB_TRANSACTION_

#include <stdint.h>

#define BOOK_FIELD_LENGTH (30)

typedef struct RentBookInfo_t
{
  char bookID[BOOK_FIELD_LENGTH];
  char memberID[BOOK_FIELD_LENGTH];
} RentBookInfo;

uint8_t LibTransaction_RentBook(RentBookInfo *rentBookInfo);
uint8_t LibTransaction_ReturnBook(char *bookID);
uint8_t LibTransaction_ReqBookList(char *memberID, char **bookList, uint8_t *numBooks);

#endif /* _LIB_TRANSACTION_ */