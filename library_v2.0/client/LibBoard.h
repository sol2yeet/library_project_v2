#ifndef _LIB_BOARD_
#define _LIB_BOARD_

#include <stdint.h>

#define BOOK_FIELD_LENGTH (30)
#define BOOK_TABLE_SIZE (100)

#define BORROWED (0)
#define AVAILABLE (1)

enum BookField
{
  BOOK_ID = 1,
  TITLE,
  AUTHOR,
  GENRE,
  BORROW_STATUS,
  BORROWER_ID
};

typedef struct Book_t
{
  char bookID[BOOK_FIELD_LENGTH];
  char title[BOOK_FIELD_LENGTH];
  char author[BOOK_FIELD_LENGTH];
  char genre[BOOK_FIELD_LENGTH];
  int borrowStatus; // (0: 대출불가, 1: 대출가능)
  char borrowerID[BOOK_FIELD_LENGTH];
} Book;

typedef struct EditBookMessage_t
{
  char bookID[BOOK_FIELD_LENGTH];
  int bookField;
  char token[BOOK_FIELD_LENGTH];
} EditBookMessage;

typedef struct RentBookInfo_t
{
  char bookID[BOOK_FIELD_LENGTH];
  char memberID[BOOK_FIELD_LENGTH];
} RentBookInfo;

void LibBoard_LoginMain(void);
void LibBoard_BookMain(void);

void LibBoard_printJoin(void);
void LibBoard_printLogin(uint8_t adminStatus);

void LibBoard_printAddBook(void);
void LibBoard_printSearchBook(void);
void LibBoard_printDeleteBook(void);
void LibBoard_printEditBook(void);

void LibBoard_TransctionMain(void);
void LibBoard_printReturnBook(void);
void LibBoard_printRentBook(void);
void LibBoard_printMybookList(void);

#endif /* _LIB_BOARD_ */