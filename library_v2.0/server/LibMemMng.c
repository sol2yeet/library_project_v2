#include "LibMemMng.h"
#include "LibMain.h"

uint8_t LibMemMng_IsPasswordMatch(char *memberID, char *memberPW);
uint8_t LibMemMng_CreateHashKey(char *memberID);
uint8_t LibMemMng_MapMemberInfo(Member *member);
uint8_t LibMemMng_SaveMemListToFile(Member *member);
void LibMemMng_MappingMemList(Member *member);

uint8_t LibMemMng_Join(Member *member)
{
  uint8_t result = FAILURE;
  result = LibMemMng_IsValiedMemberID(member->memberID);
  if (result == SUCCESS)
  {
    return FAILURE;
  }
  result = LibMemMng_MapMemberInfo(member);
  if (result != SUCCESS)
  {
    return FAILURE;
  }
  result = LibMemMng_SaveMemListToFile(member);
  if (result != SUCCESS)
  {
    return FAILURE;
  }
  return result;
}

uint8_t LibMemMng_login(char *memberID, char *memberPW)
{
  uint8_t result = FAILURE;
  result = LibMemMng_IsValiedMemberID(memberID);
  if (result != SUCCESS)
  {
    return FAILURE;
  }

  result = LibMemMng_IsPasswordMatch(memberID, memberPW);
  if (result != SUCCESS)
  {
    return FAILURE;
  }
  // result = LibMemMng_CreateHashKey(memberID);
  if (result != SUCCESS)
  {
    return FAILURE;
  }
  return result;
}

uint8_t LibMemMng_CreateHashKey(char *memberID)
{
  unsigned long hash = 5381;
  int c;

  while ((c = *memberID++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash % MEM_TABLE_SIZE;
}

uint8_t LibMemMng_IsValiedMemberID(char *memberID)
{
  LIB_MIB *pLibMib = GetLibMibPtr();
  int i;

  for (i = 0; i < pLibMib->numMembers; i++)
  {
    if (strcmp(pLibMib->memberList[i].memberID, memberID) == 0)
    {
      return SUCCESS;
    }
  }
  return FAILURE;
}

uint8_t LibMemMng_IsPasswordMatch(char *memberID, char *memberPW)
{
  LIB_MIB *pLibMib = GetLibMibPtr();
  int i;

  for (i = 0; i < pLibMib->numMembers; i++)
  {
    if (strcmp(pLibMib->memberList[i].memberID, memberID) == 0)
    {
      if (strcmp(pLibMib->memberList[i].memberPW, memberPW) == 0)
      {
        return SUCCESS;
      }
      else
      {
        return FAILURE;
      }
    }
  }
  return FAILURE;
}

uint8_t LibMemMng_IsAdmin(char *memberID)
{
  LIB_MIB *pLibMib = GetLibMibPtr();
  int i;

  for (i = 0; i < pLibMib->numMembers; i++)
  {
    if (strcmp(pLibMib->memberList[i].memberID, memberID) == 0)
    {
      if (pLibMib->memberList[i].adminStatus == SYSTEM_ADMIN)
      {
        return SYSTEM_ADMIN;
      }
      else
      {
        return GENERAL_USER;
      }
    }
  }
  return GENERAL_USER;
}

uint8_t LibMemMng_MapMemberInfo(Member *member)
{
  LIB_MIB *pLibMib = GetLibMibPtr();

  if (pLibMib->numMembers >= MEM_TABLE_SIZE)
  {
    printf("Member list is full. Cannot add more members.\n");
    return FAILURE;
  }

  pLibMib->memberList[pLibMib->numMembers] = *member;
  pLibMib->numMembers++;

  return SUCCESS;
}

uint8_t LibMemMng_SaveMemListToFile(Member *member)
{
  LIB_MIB *pLibMib = GetLibMibPtr();
  int i;

  FILE *file = fopen(MEMBER_FILE_NAME, "w");
  if (file == NULL)
  {
    perror("Error opening file");
    return FAILURE;
  }

  for (i = 0; i < pLibMib->numMembers; i++)
  {
    Member currentMember = pLibMib->memberList[i];

    if (fprintf(file, "%s %s %s %d\n", currentMember.memberID, currentMember.memberPW, currentMember.memberName, currentMember.adminStatus) < 0)
    {
      fclose(file);
      return FAILURE;
    }
  }
  fclose(file);
  return SUCCESS;
}

void DebugPrintMemberInfo(const Member *member);

void DebugPrintMemberInfo(const Member *member)
{
  printf("Member ID: %s, Password: %s, Name: %s, Admin Status: %d\n",
         member->memberID, member->memberPW, member->memberName, member->adminStatus);
}

void LibMemMng_MappingMemList(Member *member)
{
  /* 회원정보 데이터 매핑 */
  LIB_MIB *pLibMib = GetLibMibPtr();

  if (pLibMib->numMembers < MEM_TABLE_SIZE)
  {
    pLibMib->memberList[pLibMib->numMembers] = *member;
    // DebugPrintMemberInfo(&pLibMib->memberList[pLibMib->numMembers]);
    pLibMib->numMembers++;
  }
  else
  {
    printf("Member list is full. Cannot add more members.\n");
  }
}

void LibMemMng_Init(const char *filename)
{
  FILE *file = fopen(filename, "r");
  if (file == NULL)
  {
    printf("No existing member file. Creating a new one.\n");
    file = fopen(filename, "w");
    if (file == NULL)
    {
      perror("Error creating file");
      exit(EXIT_FAILURE);
    }
    fclose(file);
    return;
  }
  char id[MEM_FIELD_LENGTH], pw[MEM_FIELD_LENGTH], name[MEM_FIELD_LENGTH];
  int adminStatus = 0;

  while (fscanf(file, "%s %s %s %d", id, pw, name, &adminStatus) == 4)
  {
    Member member;
    strcpy(member.memberID, id);
    strcpy(member.memberPW, pw);
    strcpy(member.memberName, name);
    member.adminStatus = adminStatus;

    LibMemMng_MappingMemList(&member);
  }
  fclose(file);
  printf("Member information loaded from file successfully.\n");
}
