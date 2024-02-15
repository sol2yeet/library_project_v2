#include "LibMib.h"
#include "LibMain.h"

void LibMib_Init(void)
{
  LIB_MIB *pLibMib = GetLibMibPtr();

  pLibMib->numMembers = 0;
  memset(pLibMib->memberList, 0, MEM_TABLE_SIZE * sizeof(Member));
}

LIB_MIB *GetLibMibPtr(void)
{
  return &pLibMib;
}