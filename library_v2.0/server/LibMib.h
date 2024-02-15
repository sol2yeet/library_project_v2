#ifndef _LIB_MIB_
#define _LIB_MIB_

#include "LibMemMng.h"

typedef struct LIB_MIB_t
{
  Member memberList[MEM_TABLE_SIZE];
  int numMembers;
} LIB_MIB;

void LibMib_Init(void);
LIB_MIB *GetLibMibPtr(void);

static LIB_MIB pLibMib;

#endif /* _LIB_MIB_ */