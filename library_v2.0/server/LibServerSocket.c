#include "LibServerSocket.h"
#include "LibMain.h"
#include "LibMemMng.h"
#include "LibBookMng.h"
#include "LibTransaction.h"

#define RUN_TASK (1)

static struct sockaddr_in serverTxAddr;
static struct sockaddr_in serverRxAddr;
int serverSocket = 0;
int thread_rx_udp_data_exit = 0;

void *LibServer_RecvThread(void *arg);
void LibServer_ParseMsg(uint8_t *message, uint32_t msgLen);
void LibServer_SendData(const uint8_t *pdata, uint16_t msgLength);
void LibServer_sendACKmsg(LIB_ACK *ackHeader, uint8_t result, uint16_t bodyLan, uint8_t *msgData);

void LibServer_SendData(const uint8_t *pdata, uint16_t msgLength)
{
  printf("4. ack send start %d \n", msgLength);

#ifdef DEBUG_PRINT

  for (int i = 0; i < msgLength; i++)
  {
    printf("%02X ", pdata[i]);
  }
  printf("\n");
#endif

  ssize_t sentBytes = sendto(serverSocket, pdata, msgLength, 0, (struct sockaddr *)&serverTxAddr, sizeof(serverTxAddr));

  if (sentBytes < 0 || (size_t)sentBytes != msgLength)
  {
    perror("Failed to send UDP data");
  }
  else
  {
    printf("Successfully sent %zd bytes.\n", sentBytes);
  }
}

void LibServer_SocketSetup(void)
{

  printf("2. server socket init\n");

  // 소켓 생성
  if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror("Failed to create UDP socket\n");
    exit(EXIT_FAILURE);
  }

  // 서버 주소 구조체 초기화
  memset(&serverRxAddr, 0, sizeof(serverRxAddr));
  serverRxAddr.sin_family = AF_INET;
  serverRxAddr.sin_addr.s_addr = htonl(INADDR_ANY); // host -> network(모두수락)
  if (serverRxAddr.sin_addr.s_addr == INADDR_NONE)
  {
    fprintf(stderr, "Invalid IP address format\n");
    return;
  }
  serverRxAddr.sin_port = htons(SERVER_RX_PORT);

  // 소켓에 주소 할당
  if (bind(serverSocket, (struct sockaddr *)&serverRxAddr, sizeof(struct sockaddr_in)) < 0)
  {
    perror("Failed to bind UDP socket\n");
    close(serverSocket);
    exit(EXIT_FAILURE);
  }
  // 서버 주소 구조체 초기화
  memset(&serverTxAddr, 0, sizeof(serverTxAddr));
  serverTxAddr.sin_family = AF_INET;
  serverTxAddr.sin_addr.s_addr = htonl(INADDR_ANY); // host -> network(모두수락)
  if (serverTxAddr.sin_addr.s_addr == INADDR_NONE)
  {
    fprintf(stderr, "Invalid IP address format\n");
    return;
  }
  serverTxAddr.sin_port = htons(SERVER_TX_PORT);
}

void *LibServer_RecvThread(void *arg)
{
  fd_set fdset;
  uint32_t selectcnt = 0;
  uint16_t recvlen = 0;
  uint8_t buffer[1500];
  socklen_t addrLen = sizeof(serverTxAddr);

  if (arg == NULL)
  {
    printf("arg NULL\n");
  }
  printf("5. recv msg start \n");

  while (1)
  {
    FD_ZERO(&fdset);
    FD_SET(serverSocket, &fdset);
    selectcnt = select(serverSocket + 1, &fdset, NULL, NULL, NULL);

    if (selectcnt > 0 && FD_ISSET(serverSocket, &fdset))
    {
      memset(&buffer, 0, sizeof(buffer));

      // 클라이언트로부터 메시지 수신
      uint16_t recvLen = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr *)&serverTxAddr, &addrLen);
      if (recvLen < 0)
      {
        perror("data recv error");
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
      // 수신된 메시지 처리
      buffer[recvLen] = '\0';
      LibServer_ParseMsg(buffer, recvLen);
    }
    usleep(1);
    if (thread_rx_udp_data_exit != RUN_TASK)
      break;
  }

  return NULL;
}

void LibServer_ParseMsg(uint8_t *message, uint32_t msgLen)
{
  printf("6. parsing recv data \n");

  LIB_HDR *libHeader;
  uint8_t cmd;
  uint8_t result = 0;
  uint16_t bodyLan;

#ifdef DEBUG_PRINT
  printf("Received message. msgID: %02X, bodyLength: %d\n", libHeader->msgID, libHeader->bodyLength);
#endif

  libHeader = (LIB_HDR *)message;
  cmd = libHeader->msgID;

  switch (cmd)
  {
  case MEM_JOIN_REQ_MSG:
  case MEM_LOGIN_REQ_MSG:
  case ADMIN_LOGIN_REQ_MSG:
    if (libHeader->bodyLength > 0)
    {
      Member member;
      memset(&member, 0, sizeof(Member));
      memcpy(&member, &message[MSG_BODY_IDX], sizeof(Member));

      if (cmd == ADMIN_LOGIN_REQ_MSG)
      {
        if (LibMemMng_IsAdmin(member.memberID) == SYSTEM_ADMIN)
        {
          result = LibMemMng_login(member.memberID, member.memberPW);
        }
        else
        {
          result = FAILURE;
        }
      }
      else if (cmd == MEM_JOIN_REQ_MSG)
      {
        result = LibMemMng_Join(&member);
      }
      else
      {
        result = LibMemMng_login(member.memberID, member.memberPW);
      }

      printf("result : %d\n", result);
      LibServer_sendACKmsg((LIB_ACK *)libHeader, result, 0, NULL);
    }
    break;

  case BOOK_SEARCH_REQ_MSG:
    if (libHeader->bodyLength > 0)
    {
      char *matchingBooks = NULL;
      uint8_t numBooks = 0;
      char token[BOOK_FIELD_LENGTH] = {0};

      memcpy(token, &message[MSG_BODY_IDX], sizeof(token));

      if (LibBookMng_SearchBook(token, &matchingBooks, &numBooks) != 0)
      {
        result = SUCCESS;
        printf("result : %d\n", result);
        bodyLan = numBooks * sizeof(Book);
        LibServer_sendACKmsg((LIB_ACK *)libHeader, result, bodyLan, matchingBooks);
      }
      else
      {
        LibServer_sendACKmsg((LIB_ACK *)libHeader, result, 0, NULL);
      }
      // 메모리가 할당되었을 경우에만 해제
      if (matchingBooks != NULL)
      {
        free(matchingBooks);
        matchingBooks = NULL;
      }
    }
    break;

  case BOOK_ADD_REQ_MSG:
    if (libHeader->bodyLength > 0)
    {
      Book book;
      memset(&book, 0, sizeof(Book));
      memcpy(&book, &message[MSG_BODY_IDX], sizeof(Book));

      result = LibBookMng_AddBook(&book);
      printf("result : %d\n", result);
      LibServer_sendACKmsg((LIB_ACK *)libHeader, result, 0, NULL);
    }
    break;

  case BOOK_DELETE_REQ_MSG:
    if (libHeader->bodyLength > 0)
    {
      char token[BOOK_FIELD_LENGTH] = {0};
      memcpy(token, &message[MSG_BODY_IDX], sizeof(token));

      result = LibBookMng_DeleteBook(token);
      printf("result : %d\n", result);
      LibServer_sendACKmsg((LIB_ACK *)libHeader, result, 0, NULL);
    }
    break;

  case BOOK_MODIFY_REQ_MSG:
    if (libHeader->bodyLength > 0)
    {
      EditBookMessage messageBody;
      memset(&messageBody, 0, sizeof(EditBookMessage));
      memcpy(&messageBody, &message[MSG_BODY_IDX], sizeof(EditBookMessage));

      result = LibBookMng_UpdateBook(&messageBody);
      printf("result : %d\n", result);
      LibServer_sendACKmsg((LIB_ACK *)libHeader, result, 0, NULL);
    }
    break;

  case RENT_REQ_MSG:
    if (libHeader->bodyLength > 0)
    {
      RentBookInfo rentBookInfo;
      memset(&rentBookInfo, 0, sizeof(RentBookInfo));
      memcpy(&rentBookInfo, &message[MSG_BODY_IDX], sizeof(RentBookInfo));
      result = LibTransaction_RentBook(&rentBookInfo);
      printf("result : %d\n", result);
      LibServer_sendACKmsg((LIB_ACK *)libHeader, result, 0, NULL);
    }
    break;

  case RETURN_REQ_MSG:
    if (libHeader->bodyLength > 0)
    {
      char token[BOOK_FIELD_LENGTH] = {0};
      memcpy(token, &message[MSG_BODY_IDX], sizeof(token));

      result = LibTransaction_ReturnBook(token);
      printf("result : %d\n", result);
      LibServer_sendACKmsg((LIB_ACK *)libHeader, result, 0, NULL);
    }
    break;

  case MY_BOOK_LIST_REQ_MSG:
    if (libHeader->bodyLength > 0)
    {
      char *machingBooks = NULL;
      uint8_t numBooks = 0;
      char token[BOOK_FIELD_LENGTH] = {0};

      memcpy(token, &message[MSG_BODY_IDX], sizeof(token));

      if (LibTransaction_ReqBookList(token, &machingBooks, &numBooks) != 0)
      {
        result = SUCCESS;
        printf("result : %d\n", result);
        bodyLan = numBooks * sizeof(Book);
        LibServer_sendACKmsg((LIB_ACK *)libHeader, result, bodyLan, machingBooks);
      }
      else
      {
        LibServer_sendACKmsg((LIB_ACK *)libHeader, result, 0, NULL);
      }
      // 메모리가 할당되었을 경우에만 해제
      if (machingBooks != NULL)
      {
        free(machingBooks);
        machingBooks = NULL;
      }
    }
    break;

  default:
    printf("Nobody data received\n");
    break;
  }
}

void LibServer_sendACKmsg(LIB_ACK *ackHeader, uint8_t result, uint16_t bodyLan, uint8_t *msgData)
{
  uint16_t bufferSize = sizeof(LIB_ACK) + bodyLan;
  char buffer[MAX_BUFFER_LENGTH];

  if (result == SUCCESS)
    ackHeader->result = SUCCESS;
  else
    ackHeader->result = FAILURE;

  switch (ackHeader->msgID)
  {
  case BOOK_SEARCH_REQ_MSG:
    memcpy(&buffer[sizeof(LIB_ACK)], msgData, bodyLan);
    printf("Book data in buffer: %s\n", &buffer[sizeof(LIB_ACK)]);
    break;

  case MY_BOOK_LIST_REQ_MSG:
    memcpy(&buffer[sizeof(LIB_ACK)], msgData, bodyLan);
    printf("Book data in buffer: %s\n", &buffer[sizeof(LIB_ACK)]);
    break;

  default:
    break;
  }

  ackHeader->bodyLength = bodyLan;
  memcpy(&buffer[0], ackHeader, sizeof(LIB_ACK));
  LibServer_SendData((const uint8_t *)buffer, bufferSize);
}

void LibServer_RecvThreadSetup(void)
{
  int return_value_ctrl = 0;
  pthread_t serverReceiveThread;

  printf("3. server recv thread create\n");

  thread_rx_udp_data_exit = RUN_TASK;
  if (pthread_create(&serverReceiveThread, NULL, LibServer_RecvThread, NULL) != 0)
  {
    perror("pthread create error");
    close(serverSocket);
    exit(EXIT_FAILURE);
  }
}

void LibServer_Init(void)
{
  printf("1. server init \n");

  LibServer_SocketSetup();
  LibServer_RecvThreadSetup();
}