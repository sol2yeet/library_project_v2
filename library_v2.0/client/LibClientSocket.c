
#include "LibClientSocket.h"
#include "LibMain.h"
#include "LibSession.h"
#include "LibBoard.h"

#define MAX_MSG_LEN (255)
#define BOOK_FIELD_NUMBER (6)
#define MAX_BUFFER_LENGTH (1024)

void LibClient_SocketSetup(void);
void *LibClient_RecvThread(void *arg);
void LibClient_ParseMsg(uint8_t *message, uint32_t msgLen);

int clientSocket = 0;
struct sockaddr_in clientTxAddr;
struct sockaddr_in clientRxAddr;

void LibClient_sendData(const uint8_t *pdata, uint16_t msgLength)
{

  if (pdata == NULL || msgLength == 0)
  {
    printf("Invalid input data or length\n");
    return;
  }

  // printf("4. msg send start %d \n", msgLength);

#ifdef DEBUG_PRINT
  printf("Sending Data: ");
  for (int i = 0; i < msgLength; ++i)
  {
    printf("%02X ", pdata[i]);
  }
  printf("\n");
#endif

  ssize_t sentBytes = sendto(clientSocket, pdata, msgLength, 0, (struct sockaddr *)&clientTxAddr, sizeof(clientTxAddr));

  if (sentBytes < 0)
  {
    perror("Failed to send UDP data");
  }
  else
  {
    // printf("Sent %zd bytes successfully.\n", sentBytes);
  }
}

void LibClient_SocketSetup(void)
{
  // printf("2. client socket init\n");

  // 소켓 생성
  if ((clientSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror("Failed to create UDP socket\n");
    exit(EXIT_FAILURE);
  }

  // 서버 주소 구조체 초기화
  memset(&clientRxAddr, 0, sizeof(clientRxAddr));
  clientRxAddr.sin_family = AF_INET;
  clientRxAddr.sin_addr.s_addr = htonl(INADDR_ANY); // host -> network(모두수락)
  if (clientRxAddr.sin_addr.s_addr == INADDR_NONE)
  {
    fprintf(stderr, "Invalid IP address format\n");
    return;
  }
  clientRxAddr.sin_port = htons(SERVER_RX_PORT);

  // 소켓에 주소 할당
  if (bind(clientSocket, (struct sockaddr *)&clientRxAddr, sizeof(struct sockaddr_in)) < 0)
  {
    perror("inet_pton error");
    close(clientSocket);
    exit(EXIT_FAILURE);
  }

  memset(&clientTxAddr, 0, sizeof(clientTxAddr));
  clientTxAddr.sin_family = AF_INET;
  clientTxAddr.sin_addr.s_addr = htonl(INADDR_ANY); // host -> network(모두수락)
  if (clientTxAddr.sin_addr.s_addr == INADDR_NONE)
  {
    fprintf(stderr, "Invalid IP address format\n");
    return;
  }
  clientTxAddr.sin_port = htons(SERVER_TX_PORT);
}

void *LibClient_RecvThread(void *arg)
{
  uint8_t buffer[1500];
  fd_set fdset;
  uint32_t selectcnt = 0;
  socklen_t addrLen = sizeof(clientTxAddr);

  if (arg == NULL)
  {
    // printf("arg NULL\n");
  }

  // printf("5. recv msg start \n");

  while (1)
  {
    FD_ZERO(&fdset);
    FD_SET(clientSocket, &fdset);
    selectcnt = select(clientSocket + 1, &fdset, NULL, NULL, NULL);

    if (selectcnt > 0 && FD_ISSET(clientSocket, &fdset))
    {
      memset(&buffer, 0, sizeof(buffer));

      uint16_t recvLen = recvfrom(clientSocket, &buffer, sizeof(buffer), 0, (struct sockaddr *)&clientTxAddr, &addrLen);

      if (recvLen < 0)
      {
        perror("Receiving error");
        continue;
      }

#ifdef DEBUG_PRINT
      printf("Received Data : ");
      for (int i = 0; i < recvLen; ++i)
      {
        printf("%02X ", buffer[i]);
      }
      printf("\n");
#endif

      buffer[recvLen] = '\0';
      LibClient_ParseMsg(buffer, recvLen);
      memset(buffer, 0, sizeof(buffer));
    }
  }
  return NULL;
}

void LibClient_ParseMsg(uint8_t *message, uint32_t msgLen)
{
  /* 수신 메시지 파싱 */
  // printf("6. parsing recv data \n");

  LIB_ACK *ackHeader;
  uint8_t cmd;

  ackHeader = (LIB_ACK *)message;

  char bodyData[ackHeader->bodyLength];
  memcpy(&bodyData[0], &message[sizeof(LIB_ACK)], sizeof(bodyData));

#ifdef DEBUG_PRINT
  printf("Received message. msgID: %02X, bodyLength: %d\n", ackHeader->msgID, ackHeader->bodyLength);
#endif

  cmd = ackHeader->msgID;

  switch (cmd)
  {
  case MEM_JOIN_RECV_MSG:
    if (ackHeader->result == SUCCESS)
    {
      printf("\n회원가입 성공\n\n");
      LibBoard_printLogin(GENERAL_USER);
    }
    else
    {
      printf("\n사용할 수 없는 정보 \n");
      LibBoard_printJoin();
    }
    break;

  case MEM_LOGIN_RECV_MSG:
    if (ackHeader->result == SUCCESS)
    {
      printf("\n회원 로그인 성공 \n");
      LibBoard_TransctionMain();
    }
    else
    {
      printf("\n정보 불일치 \n");
      LibBoard_printLogin(GENERAL_USER);
    }
    break;

  case ADMIN_LOGIN_RECV_MSG:
    if (ackHeader->result == SUCCESS)
    {
      printf("\n관리자 로그인 성공 \n");
      LibBoard_BookMain();
    }
    else
    {
      printf("\n정보 불일치 \n");
      LibBoard_printLogin(GENERAL_USER);
    }
    break;

  case BOOK_SEARCH_RECV_MSG:
    if (ackHeader->result == SUCCESS)
    {
      int answer = 0;
      int numBooks = 0;
      char *booksData = (char *)bodyData;
      char *bookDetails[BOOK_TABLE_SIZE][BOOK_FIELD_NUMBER];
      const char *lineDelim = "\n";
      const char *fieldDelim = ",";
      char *lineContext;
      char *fieldContext;

      char *line = strtok_r(booksData, lineDelim, &lineContext);

      while (line != NULL && numBooks < BOOK_TABLE_SIZE)
      {
        char *field = strtok_r(line, fieldDelim, &fieldContext);
        int fieldIndex = 0;

        while (field != NULL && fieldIndex < BOOK_FIELD_NUMBER)
        {
          bookDetails[numBooks][fieldIndex++] = field;
          field = strtok_r(NULL, fieldDelim, &fieldContext);
        }

        if (fieldIndex == BOOK_FIELD_NUMBER)
        {
          numBooks++;
        }

        line = strtok_r(NULL, lineDelim, &lineContext);
      }

      printf("\n*************************************[ Book list ( %d ) ]*************************************\n", numBooks);
      if (numBooks != 0)
      {
        for (int j = 0; j < numBooks; j++)
        {
          printf("Book ID: %s, Title: %s, Author: %s, Genre: %s, Borrowed: %s, Borrower ID: %s\n",
                 bookDetails[j][0], bookDetails[j][1], bookDetails[j][2], bookDetails[j][3],
                 bookDetails[j][4], bookDetails[j][5]);
        }

        if (session.adminState == SYSTEM_ADMIN)
        {
          printf("\n\n해당도서를  1.삭제  2.수정  3.처음으로 : \n");
          scanf("%d", &answer);
          switch (answer)
          {
          case 1:
            LibBoard_printDeleteBook();
            break;
          case 2:
            LibBoard_printEditBook();
            break;
          case 3:
            LibBoard_BookMain();
            break;
          default:
            puts("화면에 번호를 입력해주세요");
            break;
          }
        }
        else
        {
          printf("\n\n해당도서를 1.대출  2.처음으로 : \n");
          scanf("%d", &answer);
          (answer == 1) ? LibBoard_printRentBook() : LibBoard_TransctionMain();
        }
      }
      else
      {
        printf("검색된 도서가 없습니다. 이전으로 돌아갑니다. \n");
        (session.adminState == SYSTEM_ADMIN) ? LibBoard_BookMain() : LibBoard_TransctionMain();
      }
    }
    break;

  case BOOK_ADD_RECV_MSG:
    (ackHeader->result == SUCCESS) ? printf("도서추가 성공\n")
                                   : printf("사용할 수 없는 정보 \n");
    LibBoard_BookMain();
    break;

  case BOOK_DELETE_RECV_MSG:
    (ackHeader->result == SUCCESS) ? printf("도서삭제 성공\n")
                                   : printf("사용할 수 없는 정보 \n");
    LibBoard_BookMain();
    break;

  case BOOK_MODIFY_RECV_MSG:
    (ackHeader->result == SUCCESS) ? printf("도서수정 성공\n")
                                   : printf("사용할 수 없는 정보 \n");
    LibBoard_BookMain();
    break;

  case MY_BOOK_LIST_RECV_MSG:
    if (ackHeader->result == SUCCESS)
    {
      int answer = 0;
      int numBooks = 0;
      char *booksData = (char *)bodyData;
      char *bookDetails[BOOK_TABLE_SIZE][BOOK_FIELD_NUMBER];
      const char *lineDelim = "\n";
      const char *fieldDelim = ",";
      char *lineContext;
      char *fieldContext;

      char *line = strtok_r(booksData, lineDelim, &lineContext);

      while (line != NULL && numBooks < BOOK_TABLE_SIZE)
      {
        char *field = strtok_r(line, fieldDelim, &fieldContext);
        int fieldIndex = 0;

        while (field != NULL && fieldIndex < BOOK_FIELD_NUMBER)
        {
          bookDetails[numBooks][fieldIndex++] = field;
          field = strtok_r(NULL, fieldDelim, &fieldContext);
        }

        if (fieldIndex == BOOK_FIELD_NUMBER)
        {
          numBooks++;
        }

        line = strtok_r(NULL, lineDelim, &lineContext);
      }

      printf("\n********************************[ Book list ( %d ) ]********************************\n", numBooks);
      if (numBooks != 0)
      {
        for (int j = 0; j < numBooks; j++)
        {
          printf("Book ID: %s, Title: %s, Author: %s, Genre: %s\n",
                 bookDetails[j][0], bookDetails[j][1], bookDetails[j][2], bookDetails[j][3]);
        }
        printf("\n\n해당도서를 1.반납   2.이전으로 : \n");
        scanf("%d", &answer);
        (answer == 1) ? LibBoard_printReturnBook() : LibBoard_TransctionMain();
      }
      else
      {
        printf("대여중인 도서가 없습니다. 이전으로 돌아갑니다. \n");
        LibBoard_TransctionMain();
      }
    }
    else
    {
      printf("검색실패. 다시입력해주세요. \n");
      LibBoard_TransctionMain();
    }
    break;

  case RETURN_RECV_MSG:
    (ackHeader->result == SUCCESS) ? printf("도서반납 성공\n")
                                   : printf("사용할 수 없는 정보 \n");
    LibBoard_TransctionMain();
    break;

  case RENT_RECV_MSG:
    (ackHeader->result == SUCCESS) ? printf("도서대출 성공\n")
                                   : printf("사용할 수 없는 정보 \n");
    LibBoard_TransctionMain();
    break;

  default:
    printf("No body data recv\n");
    break;
  }
}

void LibClient_RecvThreadSetup(void)
{
  int return_value_ctrl = 0;
  static pthread_t clientReceiveThread = 0;

  // printf("3. client recv thread create\n");

  if (pthread_create(&clientReceiveThread, NULL, LibClient_RecvThread, NULL) != 0)
  {
    perror("pthread create error\n");
    close(clientSocket);
    exit(EXIT_FAILURE);
  }
}

void LibClient_Init()
{
  // printf("1. client init \n");

  LibClient_SocketSetup();
  LibClient_RecvThreadSetup();
}
