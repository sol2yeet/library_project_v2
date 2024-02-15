#include "LibMain.h"
#include "LibSession.h"
#include "LibClientSocket.h"
#include "LibBoard.h"

/*
(1)관리자 -> 회원가입 불가, 파일에 직접등록해야 관리자 권한이 부여됨
            -> [기능] 등록된 도서 수정(검색, 삭제, 수정), 도서추가

(0)회원   -> 회원가입시 일반회원으로 자동등록
            -> [기능] 대출(검색후 대출), 반납(나의 대출목록 확인후 반납)
*/

int main(int argc, char *argv[])
{
  LibClient_Init();
  LibSession_Init();

  LibBoard_LoginMain();

  while (1)
  {
    ;
  }

  return 0;
}
