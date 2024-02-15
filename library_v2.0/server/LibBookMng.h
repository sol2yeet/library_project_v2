#ifndef _LIB_BOOK_MNG_
#define _LIB_BOOK_MNG_

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

typedef struct BookNode_t
{
  Book book;
  struct BookNode_t *next;
} BookNode;

typedef struct EditBookMessage_t
{
  char bookID[BOOK_FIELD_LENGTH];
  int bookField;
  char token[BOOK_FIELD_LENGTH];
} EditBookMessage;

void LibBookMng_Init(const char *filename);
uint8_t LibBookMng_UpdateBook(EditBookMessage *messageBody);
uint8_t LibBookMng_SearchBook(char *inputData, char **matchingBooks, uint8_t *numBooks);
uint8_t LibBookMng_IsValidBookID(char *bookID);
uint8_t LibBookMng_IsBorrowedBook(char *bookID);
uint8_t LibBookMng_AddBook(Book *book);
uint8_t LibBookMng_DeleteBook(char *bookID);

#endif /* _LIB_BOOK_MNG_ */