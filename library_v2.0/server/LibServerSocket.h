#ifndef _LIB_SERVER_SOCKET_
#define _LIB_SERVER_SOCKET_

#include <stdint.h>

#define SERVER_TX_PORT (9000)
#define SERVER_RX_PORT (9100)

/* 클라이언트 요청 메시지 정의 */
#define MEM_JOIN_REQ_MSG (0x01)
#define MEM_LOGIN_REQ_MSG (0x02)
#define ADMIN_LOGIN_REQ_MSG (0x03)
#define BOOK_SEARCH_REQ_MSG (0x04)
#define BOOK_MODIFY_REQ_MSG (0x05)
#define BOOK_DELETE_REQ_MSG (0x06)
#define BOOK_ADD_REQ_MSG (0x07)
#define RENT_REQ_MSG (0x08)
#define RETURN_REQ_MSG (0x09)
#define MY_BOOK_LIST_REQ_MSG (0x10)

#define LIB_HDR_SIZE (4)
#define MSG_BODY_IDX (4)

typedef struct LIB_HDR_t
{
  uint8_t msgID;
  uint8_t memberID;
  uint16_t bodyLength;
} LIB_HDR;

typedef struct LIB_ACK_t
{
  uint8_t msgID;
  uint8_t result;
  uint16_t bodyLength;
} LIB_ACK;

void LibServer_Init(void);
void LibServer_SocketSetup(void);

#endif /* _LIB_SERVER_SOCKET_ */