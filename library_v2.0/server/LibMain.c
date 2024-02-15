#include "LibMain.h"
#include "LibServerSocket.h"
#include "LibBookMng.h"
#include "LibMemMng.h"

int main(int argc, char *argv[])
{
  LibMib_Init();
  LibBookMng_Init(BOOK_FILE_NAME);
  LibMemMng_Init(MEMBER_FILE_NAME);

  LibServer_Init();

  while (1)
  {
    ;
  }

  return 0;
}