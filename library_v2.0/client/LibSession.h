
#ifndef _LIB_SESSION_
#define _LIB_SESSION_

#include <stdint.h>
#include <time.h>

#define MEM_FIELD_LENGTH (20)

#define GENERAL_USER (0)
#define SYSTEM_ADMIN (1)

typedef struct Session_t
{
  uint8_t adminState;
  time_t lastActiveTime; // 타입 수정
} Session;

typedef struct Member_t
{
  char memberID[MEM_FIELD_LENGTH];
  char memberPW[MEM_FIELD_LENGTH];
  char memberName[MEM_FIELD_LENGTH];
  uint8_t adminStatus; // (0: 일반 사용자, 1: 관리자)
} Member;

extern Session session;

void LibSession_Init(void);
void LibClient_UpdateActivityTime(void);

#endif /* _LIB_SESSION_ */