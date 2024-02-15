#ifndef _LIB_CLIENT_SOCKET_
#define _LIB_CLIENT_SOCKET_

#include <stdint.h>

#define SERVER_IP "127.0.0.1" // 서버의 IP 주소

#define SERVER_TX_PORT (9100)
#define SERVER_RX_PORT (9000)

/* 서버 응답 메시지 정의 */
#define CMD_RECV_MSG (0x0)
#define MEM_JOIN_RECV_MSG (0x01)
#define MEM_LOGIN_RECV_MSG (0x02)
#define ADMIN_LOGIN_RECV_MSG (0x03)
#define BOOK_SEARCH_RECV_MSG (0x04)
#define BOOK_MODIFY_RECV_MSG (0x05)
#define BOOK_DELETE_RECV_MSG (0x06)
#define BOOK_ADD_RECV_MSG (0x07)
#define RENT_RECV_MSG (0x08)
#define RETURN_RECV_MSG (0x09)
#define MY_BOOK_LIST_RECV_MSG (0x10)

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

void LibClient_Init(void);
void LibClient_sendData(const uint8_t *pdata, uint16_t msgLength);

#endif /* _LIB_CLIENT_SOCKET_ */