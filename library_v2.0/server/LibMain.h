#ifndef _LIB_MAIN_
#define _LIB_MAIN_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include "LibMib.h"

#define BOOK_FILE_NAME "Book.txt"
#define MEMBER_FILE_NAME "Member.txt"

// #define DEBUG_PRINT

/* 응답메시지 */
#define FAILURE (0)
#define SUCCESS (1)

#define MAX_BUFFER_LENGTH (2048)

#endif /* _LIB_MAIN_ */