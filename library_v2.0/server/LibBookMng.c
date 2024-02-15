#include "LibBookMng.h"
#include "LibMain.h"
#include "LibMemMng.h"

uint8_t LibBookMng_IsDuplicateTitle(char *title);
uint8_t LibBookMng_AddBookToList(Book *book);
uint8_t LibBookMng_DeleteBookToList(char *bookID);
uint8_t LibBookMng_EditBookInfoField(char *bookID, int fieldNumber, char *newValue);
uint8_t LibBookMng_SaveBookListToFile(const char *filename);
void LibBookMng_MappingBookList(Book *book);
uint8_t LibBookMng_GetAdminState(char *memberID);
void LibBookMng_PrintBookList(void); /* 디버깅 함수 */
int LibBookMng_BookNodeSize(void);   /* 디버깅 함수 */

BookNode *bookList = NULL;

uint8_t LibBookMng_SearchBook(char *inputData, char **matchingBooks, uint8_t *numBooks)
{
  int bufferSize = 0;
  *numBooks = 0;

  BookNode *current = bookList;
  while (current != NULL)
  {
    if (strstr(current->book.title, inputData) != NULL ||
        strstr(current->book.author, inputData) != NULL ||
        strstr(current->book.genre, inputData) != NULL ||
        strstr(current->book.borrowerID, inputData) != NULL)
    {
      char tempBuffer[MAX_BUFFER_LENGTH] = {0};

      sprintf(tempBuffer, "%s,%s,%s,%s,%d,%s\n",
              current->book.bookID, current->book.title, current->book.author, current->book.genre,
              current->book.borrowStatus, current->book.borrowerID);

      // realloc 호출 전
      printf("Reallocating memory: Current bufferSize = %d\n", bufferSize);

      int tempBufferSize = strlen(tempBuffer);
      char *newMemory = realloc(*matchingBooks, bufferSize + tempBufferSize + 1);
      if (newMemory == NULL)
      {
        printf("Failed to reallocate memory\n");
        // 실패 시 이미 할당된 메모리 해제
        if (*matchingBooks != NULL)
        {
          free(*matchingBooks);
          *matchingBooks = NULL;
        }
        return FAILURE;
      }

      *matchingBooks = newMemory;
      strcpy(*matchingBooks + bufferSize, tempBuffer); // strcat 대신 사용
      bufferSize += tempBufferSize;
      (*numBooks)++;
      printf("Memory reallocation successful: New bufferSize = %d\n", bufferSize + 1);
    }
    current = current->next;
  }
  if (*matchingBooks == NULL)
  { // 일치하는 책이 없을 경우
    return FAILURE;
  }
  return SUCCESS;
}

uint8_t LibBookMng_UpdateBook(EditBookMessage *messageBody)
{
  uint8_t result = FAILURE;

  result = LibBookMng_IsValidBookID(messageBody->bookID);
  if (result != SUCCESS)
  {
    return FAILURE;
  }
  result = LibBookMng_EditBookInfoField(messageBody->bookID, messageBody->bookField, messageBody->token);
  if (result != SUCCESS)
  {
    return FAILURE;
  }
  result = LibBookMng_SaveBookListToFile(BOOK_FILE_NAME);
  if (result != SUCCESS)
  {
    return FAILURE;
  }
  return result;
}

uint8_t LibBookMng_AddBook(Book *book)
{
  uint8_t result = FAILURE;

  result = LibBookMng_AddBookToList(book);
  if (result != SUCCESS)
  {
    return FAILURE;
  }
  result = LibBookMng_SaveBookListToFile(BOOK_FILE_NAME);
  if (result != SUCCESS)
  {
    return FAILURE;
  }
  return result;
}

uint8_t LibBookMng_DeleteBook(char *bookID)
{
  uint8_t result = FAILURE;

  result = LibBookMng_DeleteBookToList(bookID);
  if (result != SUCCESS)
  {
    return FAILURE;
  }
  result = LibBookMng_SaveBookListToFile(BOOK_FILE_NAME);
  if (result != SUCCESS)
  {
    return FAILURE;
  }
  return result;
}

uint8_t LibBookMng_IsValidBookID(char *bookID)
{
  BookNode *current = bookList;

  while (current != NULL)
  {
    if (strcmp(current->book.bookID, bookID) == 0)
    {
      return SUCCESS;
    }
    current = current->next;
  }
  return FAILURE;
}

uint8_t LibBookMng_IsDuplicateTitle(char *title)
{
  BookNode *current = bookList;

  while (current != NULL)
  {
    if (strcmp(current->book.title, title) == 0)
    {
      return SUCCESS;
    }
    current = current->next;
  }
  return FAILURE;
}

uint8_t LibBookMng_IsBorrowedBook(char *bookID)
{
  BookNode *current = bookList;

  while (current != NULL)
  {
    if (strcmp(current->book.bookID, bookID) == 0)
    {
      return current->book.borrowStatus;
    }
    current = current->next;
  }
}

uint8_t LibBookMng_EditBookInfoField(char *bookID, int fieldNumber, char *newValue)
{
  BookNode *current = bookList;

  while (current != NULL)
  {
    if (strcmp(current->book.bookID, bookID) == 0)
    {
      switch (fieldNumber)
      {
      case BOOK_ID:
        strcpy(current->book.bookID, newValue);
        break;
      case TITLE:
        strcpy(current->book.title, newValue);
        break;
      case AUTHOR:
        strcpy(current->book.author, newValue);
        break;
      case GENRE:
        strcpy(current->book.genre, newValue);
        break;
      case BORROW_STATUS:
        current->book.borrowStatus = atoi(newValue);
        break;
      case BORROWER_ID:
        strcpy(current->book.borrowerID, newValue);
        break;
      default:
        break;
      }
      return SUCCESS;
    }
    current = current->next;
  }
  return FAILURE;
}

uint8_t LibBookMng_DeleteBookToList(char *bookID)
{
  BookNode *current = bookList;
  BookNode *prev = NULL;

  while (current != NULL)
  {
    if (strcmp(current->book.bookID, bookID) == 0)
    {
      (prev == NULL) ? (bookList = current->next) : (prev->next = current->next);
      free(current);
      return SUCCESS;
    }
    prev = current;
    current = current->next;
  }
  return FAILURE;
}

uint8_t LibBookMng_AddBookToList(Book *book)
{
  BookNode *newBook = (BookNode *)malloc(sizeof(BookNode));
  if (newBook == NULL)
  {
    perror("Error allocating memory");
    return FAILURE;
  }
  strcpy(newBook->book.bookID, book->bookID);
  strcpy(newBook->book.title, book->title);
  strcpy(newBook->book.author, book->author);
  strcpy(newBook->book.genre, book->genre);
  newBook->book.borrowStatus = AVAILABLE;
  strcpy(newBook->book.borrowerID, book->borrowerID);

  newBook->next = bookList;
  bookList = newBook;
  return SUCCESS;
}

uint8_t LibBookMng_SaveBookListToFile(const char *filename)
{
  FILE *file = fopen(filename, "w");
  if (file == NULL)
  {
    perror("Error opening file");
    return FAILURE;
  }
  BookNode *current = bookList;
  while (current != NULL)
  {
    if (fprintf(file, "%s %s %s %s %d %s\n",
                current->book.bookID, current->book.title, current->book.author, current->book.genre,
                current->book.borrowStatus, current->book.borrowerID) < 0)
    {
      perror("Error writing to file");
      fclose(file);
      return FAILURE;
    }
    current = current->next;
  }
  fclose(file);
  return SUCCESS;
}

uint8_t LibBookMng_GetAdminState(char *memberID)
{
  uint8_t result = FAILURE;
  result = LibMemMng_IsAdmin(memberID);
  return result;
}

void LibBookMng_MappingBookList(Book *book)
{
  /* 도서정보 데이터 매핑 */
  BookNode *newBookNode = (BookNode *)malloc(sizeof(BookNode));
  if (newBookNode == NULL)
  {
    perror("Error allocating memory");
    exit(EXIT_FAILURE);
  }

  strcpy(newBookNode->book.bookID, book->bookID);
  strcpy(newBookNode->book.title, book->title);
  strcpy(newBookNode->book.author, book->author);
  strcpy(newBookNode->book.genre, book->genre);
  newBookNode->book.borrowStatus = book->borrowStatus;
  strcpy(newBookNode->book.borrowerID, book->borrowerID);

  newBookNode->next = bookList;
  bookList = newBookNode;
}

void LibBookMng_Init(const char *filename)
{
  FILE *file = fopen(filename, "r");
  if (file == NULL)
  {
    printf("No existing Book file. Creating a new one.\n");
    file = fopen(filename, "w");
    if (file == NULL)
    {
      perror("Error creating file");
      exit(EXIT_FAILURE);
    }
    fclose(file);
  }

  // (파일 형식: bookid title author genre borrowStatus borrowerID)
  char bookID[BOOK_FIELD_LENGTH], title[BOOK_FIELD_LENGTH], author[BOOK_FIELD_LENGTH], genre[BOOK_FIELD_LENGTH];
  int borrowStatus = 0;
  char borrowerID[BOOK_FIELD_LENGTH];

  while (fscanf(file, "%s %s %s %s %d %s", bookID, title, author, genre, &borrowStatus, borrowerID) == 6)
  {
    Book book;
    strcpy(book.bookID, bookID);
    strcpy(book.title, title);
    strcpy(book.author, author);
    strcpy(book.genre, genre);
    book.borrowStatus = borrowStatus;
    strcpy(book.borrowerID, borrowerID);

    LibBookMng_MappingBookList(&book);
  }
  fclose(file);
  printf("Book information loaded from file successfully.\n");

  // LibBookMng_PrintBookList();
}

void LibBookMng_PrintBookList(void)
{
  BookNode *current = bookList;
  printf("Book Node Size : %d\n", LibBookMng_BookNodeSize());
  while (current != NULL)
  {
    printf("Book ID: %s, Title: %s, Author: %s, Genre: %s, Borrowed: %d, Borrower ID: %s\n",
           current->book.bookID, current->book.title, current->book.author, current->book.genre,
           current->book.borrowStatus, current->book.borrowerID);
    current = current->next;
  }
}

int LibBookMng_BookNodeSize(void)
{
  BookNode *ptr = bookList;
  int count = 0;
  while (ptr != NULL)
  {
    count++;
    ptr = ptr->next;
  }
  return count;
}