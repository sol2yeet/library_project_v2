#include "LibTransaction.h"
#include "LibMain.h"
#include "LibBookMng.h"
#include "LibMemMng.h"

uint8_t LibTransaction_IsBookValied(char *bookID);
uint8_t LibTransaction_IsBookBorrowAvailable(char *bookID);
uint8_t LibTransaction_IsValiedMemId(char *memberID);
uint8_t LibTransaction_UpdateBookBorrowState(char *bookID, int borrowStatus);
uint8_t LibTransaction_UpdateBookborrowerID(char *bookID, char *memberID);
uint8_t LibTransaction_DeleteBookborrowerID(char *bookID);

uint8_t LibTransaction_RentBook(RentBookInfo *rentBookInfo)
{
  uint8_t result = FAILURE;

  result = LibTransaction_IsBookValied(rentBookInfo->bookID);
  if (result != SUCCESS)
  {
    return FAILURE;
  }

  result = LibTransaction_IsBookBorrowAvailable(rentBookInfo->bookID);
  if (result != SUCCESS)
  {
    return FAILURE;
  }

  result = LibTransaction_UpdateBookBorrowState(rentBookInfo->bookID, AVAILABLE);
  if (result != SUCCESS)
  {
    return FAILURE;
  }

  result = LibTransaction_UpdateBookborrowerID(rentBookInfo->bookID, rentBookInfo->memberID);
  if (result != SUCCESS)
  {
    return FAILURE;
  }

  return result;
}

uint8_t LibTransaction_ReqBookList(char *memberID, char **bookList, uint8_t *numBooks)
{
  uint8_t result = FAILURE;
  result = LibBookMng_SearchBook(memberID, bookList, numBooks);
  if (*bookList == NULL)
  {
    result = FAILURE;
  }
  return result;
}

uint8_t LibTransaction_ReturnBook(char *bookID)
{
  uint8_t result = FAILURE;

  result = LibTransaction_IsBookValied(bookID);
  if (result != SUCCESS)
  {
    return FAILURE;
  }

  result = LibTransaction_UpdateBookBorrowState(bookID, BORROWED);
  if (result != SUCCESS)
  {
    return FAILURE;
  }

  result = LibTransaction_DeleteBookborrowerID(bookID);
  if (result != SUCCESS)
  {
    return FAILURE;
  }

  return result;
}

uint8_t LibTransaction_IsBookValied(char *bookID)
{
  uint8_t result = FAILURE;
  result = LibBookMng_IsValidBookID(bookID);
  return result;
}

uint8_t LibTransaction_IsBookBorrowAvailable(char *bookID)
{
  uint8_t result = FAILURE;
  result = LibBookMng_IsBorrowedBook(bookID);
  return result;
}

uint8_t LibTransaction_IsValiedMemId(char *memberID)
{
  uint8_t result = FAILURE;
  result = LibMemMng_IsValiedMemberID(memberID);
  return result;
}

uint8_t LibTransaction_UpdateBookBorrowState(char *bookID, int borrowStatus)
{
  uint8_t result = FAILURE;
  EditBookMessage messageBody;
  memset(&messageBody, 0, sizeof(EditBookMessage));

  strncpy(messageBody.bookID, bookID, sizeof(messageBody.bookID) - 1);
  messageBody.bookField = BORROW_STATUS;
  messageBody.token[0] = borrowStatus ? '0' : '1';

  result = LibBookMng_UpdateBook(&messageBody);

  return result;
}

uint8_t LibTransaction_UpdateBookborrowerID(char *bookID, char *memberID)
{
  uint8_t result = FAILURE;
  EditBookMessage messageBody;
  memset(&messageBody, 0, sizeof(EditBookMessage));

  strncpy(messageBody.bookID, bookID, sizeof(messageBody.bookID) - 1);
  strncpy(messageBody.token, memberID, sizeof(messageBody.token) - 1);
  messageBody.bookField = BORROWER_ID;

  result = LibBookMng_UpdateBook(&messageBody);
  return result;
}

uint8_t LibTransaction_DeleteBookborrowerID(char *bookID)
{
  uint8_t result = FAILURE;

  EditBookMessage messageBody;
  memset(&messageBody, 0, sizeof(EditBookMessage));

  strncpy(messageBody.bookID, bookID, sizeof(messageBody.bookID) - 1);
  messageBody.bookField = BORROWER_ID;
  messageBody.token[0] = '0';

  result = LibBookMng_UpdateBook(&messageBody);
  return result;
}
