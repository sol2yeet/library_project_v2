#ifndef _LIB_MEM_MNG_
#define _LIB_MEM_MNG_

#include <stdint.h>

#define MEM_FIELD_LENGTH (20)
#define MEM_TABLE_SIZE (100)

#define GENERAL_USER (0)
#define SYSTEM_ADMIN (1)

typedef struct Member_t
{
  char memberID[MEM_FIELD_LENGTH];
  char memberPW[MEM_FIELD_LENGTH];
  char memberName[MEM_FIELD_LENGTH];
  uint8_t adminStatus; // (0: 일반 사용자, 1: 관리자)
} Member;

typedef struct MemberNode_t
{
  Member member;
  struct MemberNode_t *next;
} MemberNode;

struct MemberNode_t *memberTable[MEM_TABLE_SIZE];

uint8_t LibMemMng_Join(Member *member);
uint8_t LibMemMng_login(char *memberID, char *memberPW);
void LibMemMng_Init(const char *filename);
uint8_t LibMemMng_IsValiedMemberID(char *memberID);
uint8_t LibMemMng_IsAdmin(char *memberID);

#endif /* _LIB_MEM_MNG_ */