#include "LibBoard.h"
#include "LibMain.h"
#include "LibClientSocket.h"
#include "LibSession.h"

void LibBoard_LoginMain(void)
{
  int answer = 0;

  printf("\n\n");
  printf("--------------도서관 메인------------------\n");
  printf("|                                         |\n");
  printf("|             1. 회원 로그인              |\n");
  printf("|             2. 회원 가입                |\n");
  printf("|             3. 관리자 페이지            |\n");
  printf("|             4. EXIT --->                |\n");
  printf("|                                         |\n");
  printf("|-----------------------------------------|\n");
  printf("입력 : ");
  scanf(" %d", &answer);

  switch (answer)
  {
  case 1:
    printf("\n");
    printf("|>> MAIN >>------Membership--------------|\n");
    printf("|                                        |\n");
    printf("|          회원 로그인 페이지 입니다     |\n");
    printf("|                                        |\n");
    printf("|----------------------------------------|\n");
    LibBoard_printLogin(GENERAL_USER);
    break;

  case 2:
    printf("\n");
    printf("|>> MAIN >>------Membership--------------|\n");
    printf("|                                        |\n");
    printf("|         새로운 회원 등록 페이지 입니다     |\n");
    printf("|                                        |\n");
    printf("|----------------------------------------|\n");
    LibBoard_printJoin();
    break;

  case 3:
    printf("\n");
    printf("|>> MAIN >>------Membership--------------|\n");
    printf("|                                        |\n");
    printf("|      관리자 정보를 인증 해주세요       |\n");
    printf("|                                        |\n");
    printf("|----------------------------------------|\n");
    LibBoard_printLogin(SYSTEM_ADMIN);
    break;

  case 4:
    printf("\n 종료합니다. \n");
    exit(0);
    break;

  default:
    printf(" 화면에 표시된 번호를 입력해 주세요.");
    LibBoard_LoginMain();
    break;
  }
}

void LibBoard_BookMain(void)
{
  int answer = 0;

  printf("\n\n");
  printf("|>> MAIN >>-----------BOOK----------------|\n");
  printf("|                                         |\n");
  printf("|             1. 등록된 도서 수정         |\n");
  printf("|             2. 새로운 도서 추가         |\n");
  printf("|             3. MAIN --->                |\n");
  printf("|                                         |\n");
  printf("|-----------------------------------------|\n");
  session.adminState = SYSTEM_ADMIN;
  printf("입력 : ");
  scanf(" %d", &answer);

  switch (answer)
  {
  case 1:
    printf("\n");
    printf("|>> MAIN >>---------BOOK-----------------|\n");
    printf("|                                        |\n");
    printf("|        등록된 도서를 검색합니다        |\n");
    printf("|                                        |\n");
    printf("|----------------------------------------|\n");
    LibBoard_printSearchBook();
    break;

  case 2:
    printf("\n");
    printf("|>> MAIN >>---------BOOK-----------------|\n");
    printf("|                                        |\n");
    printf("|        새로운 도서를 추가합니다        |\n");
    printf("|                                        |\n");
    printf("|----------------------------------------|\n");
    LibBoard_printAddBook();
    break;

  case 3:
    LibBoard_LoginMain();
    break;

  default:
    printf(" 화면에 표시된 번호를 입력해 주세요.");
    LibBoard_BookMain();
    break;
  }
}

void LibBoard_TransctionMain(void)
{
  int answer = 0;

  printf("\n\n");
  printf("|>> MAIN >>-------TRANSACTION-------------|\n");
  printf("|                                         |\n");
  printf("| 1. 도서 대출                            |\n");
  printf("| 2. 도서 반납                            |\n");
  printf("| 3. MAIN --->                            |\n");
  printf("|                                         |\n");
  printf("|-----------------------------------------|\n");
  session.adminState = GENERAL_USER;
  printf("입력 : ");
  scanf(" %d", &answer);

  switch (answer)
  {
  case 1:
    printf("\n");
    printf("|>> MAIN >>-------TRANSACTION-------------|\n");
    printf("|                                         |\n");
    printf("| 회원의 도서 대출 페이지 입니다          |\n");
    printf("| 대출을 원하는 도서의 정보를 검색해주세요|\n");
    printf("|                                         |\n");
    printf("|-----------------------------------------|\n");
    LibBoard_printSearchBook();
    break;

  case 2:
    printf("\n");
    printf("|>> MAIN >>-------TRANSACTION-------------|\n");
    printf("|                                         |\n");
    printf("| 회원의 도서 반납 페이지 입니다          |\n");
    printf("| ID를 입력하고 대출 목록을 확인해주세요  |\n");
    printf("|                                         |\n");
    printf("|-----------------------------------------|\n");
    LibBoard_printMybookList();
    break;

  case 3:
    LibBoard_LoginMain();
    break;

  default:
    printf(" 화면에 표시된 번호를 입력해 주세요.\n");
    LibBoard_TransctionMain();
    break;
  }
}

void LibBoard_printJoin(void)
{
  Member member;
  LIB_HDR libHdr;
  memset(&libHdr, 0, sizeof(LIB_HDR));
  memset(&member, 0, sizeof(Member));
  char buffer[sizeof(LIB_HDR) + sizeof(Member)] = {0};

  puts(" ---- 회원가입 정보 입력 ----- ");
  printf("등록할 ID: ");
  scanf("%19s", member.memberID);

  printf("등록할 PW: ");
  scanf("%19s", member.memberPW);

  printf("이름: ");
  scanf("%19s", member.memberName);

  member.adminStatus = GENERAL_USER; // 관리자는 회원가입 불가

  libHdr.msgID = MEM_JOIN_RECV_MSG;
  libHdr.memberID = 0;
  libHdr.bodyLength = sizeof(Member);

  memcpy(&buffer[0], &libHdr, sizeof(LIB_HDR));
  memcpy(&buffer[sizeof(LIB_HDR)], &member, sizeof(Member));

  LibClient_sendData((const uint8_t *)buffer, sizeof(LIB_HDR) + libHdr.bodyLength);
}

void LibBoard_printLogin(uint8_t adminStatus)
{
  Member member;
  LIB_HDR libHdr;
  memset(&libHdr, 0, sizeof(LIB_HDR));
  memset(&member, 0, sizeof(Member));
  char buffer[sizeof(LIB_HDR) + sizeof(Member)] = {0};

  puts(" ---- 회원로그인 정보 입력 ----- ");
  printf("회원 ID: ");
  scanf("%19s", member.memberID);

  printf("회원 PW: ");
  scanf("%19s", member.memberPW);

  member.adminStatus = adminStatus;

  if (adminStatus == SYSTEM_ADMIN)
    libHdr.msgID = ADMIN_LOGIN_RECV_MSG;
  else
    libHdr.msgID = MEM_LOGIN_RECV_MSG;

  libHdr.memberID = 0;
  libHdr.bodyLength = sizeof(Member);

  memcpy(&buffer[0], &libHdr, sizeof(LIB_HDR));
  memcpy(&buffer[sizeof(LIB_HDR)], &member, sizeof(Member));

  LibClient_sendData((const uint8_t *)buffer, sizeof(LIB_HDR) + libHdr.bodyLength);
}

void LibBoard_printAddBook(void)
{
  LIB_HDR libHdr;
  Book book;
  memset(&libHdr, 0, sizeof(LIB_HDR));
  memset(&book, 0, sizeof(Book));
  char buffer[sizeof(LIB_HDR) + sizeof(Book)] = {0};

  puts(" ---- 등록할 도서정보 입력----- ");
  printf("책ID(영문자3자리+숫자3자리) : ");
  scanf("%29s", book.bookID);

  printf("제목: ");
  scanf("%29s", book.title);

  printf("작가: ");
  scanf("%29s", book.author);

  printf("장르: ");
  scanf("%29s", book.genre);

  book.borrowStatus = AVAILABLE;
  book.borrowerID[0] = '0';

  libHdr.msgID = BOOK_ADD_RECV_MSG;
  libHdr.bodyLength = sizeof(Book);

  memcpy(&buffer[0], &libHdr, sizeof(LIB_HDR));
  memcpy(&buffer[sizeof(LIB_HDR)], &book, sizeof(Book));

  LibClient_sendData((const uint8_t *)buffer, sizeof(LIB_HDR) + libHdr.bodyLength);
}

void LibBoard_printSearchBook(void)
{
  LIB_HDR libHdr;
  memset(&libHdr, 0, sizeof(LIB_HDR));
  char token[BOOK_FIELD_LENGTH] = {0};
  char buffer[sizeof(LIB_HDR) + sizeof(Book)] = {0};

  puts(" ---- 검색할 책의 제목,저자,장르 등 입력 ----- ");
  printf("검색 키워드 : ");
  scanf("%29s", token);

  libHdr.msgID = BOOK_SEARCH_RECV_MSG;
  libHdr.bodyLength = sizeof(token);

  memcpy(&buffer[0], &libHdr, sizeof(LIB_HDR));
  memcpy(&buffer[sizeof(LIB_HDR)], token, sizeof(token));

  LibClient_sendData((const uint8_t *)buffer, sizeof(LIB_HDR) + libHdr.bodyLength);
}

void LibBoard_printDeleteBook(void)
{
  LIB_HDR libHdr;
  memset(&libHdr, 0, sizeof(LIB_HDR));
  char token[BOOK_FIELD_LENGTH] = {0};
  char buffer[sizeof(LIB_HDR) + sizeof(token)] = {0};

  puts(" ---- 삭제할 도서정보 입력 ----- ");
  printf("책ID : ");
  scanf("%29s", token);

  libHdr.msgID = BOOK_DELETE_RECV_MSG;
  libHdr.bodyLength = sizeof(token);

  memcpy(&buffer[0], &libHdr, sizeof(LIB_HDR));
  memcpy(&buffer[sizeof(LIB_HDR)], token, sizeof(token));

  LibClient_sendData((const uint8_t *)buffer, sizeof(LIB_HDR) + libHdr.bodyLength);
}

void LibBoard_printEditBook(void)
{
  LIB_HDR libHdr;
  EditBookMessage messageBody;
  memset(&libHdr, 0, sizeof(LIB_HDR));
  memset(&messageBody, 0, sizeof(EditBookMessage));
  char buffer[sizeof(LIB_HDR) + sizeof(EditBookMessage)] = {0};

  puts(" ---- 수정할 도서정보 입력 ----- ");
  printf("책ID : ");
  scanf("%29s", messageBody.bookID);

  printf("수정할 항목을 선택하세요\n1.책ID  2.제목  3.작가  4.장르  :");
  scanf("%d", &messageBody.bookField);

  printf("수정할 내용을 입력하세요 : ");
  scanf("%29s", messageBody.token);

  libHdr.msgID = BOOK_MODIFY_RECV_MSG;
  libHdr.bodyLength = sizeof(EditBookMessage);

  memcpy(&buffer[0], &libHdr, sizeof(LIB_HDR));
  memcpy(&buffer[sizeof(LIB_HDR)], &messageBody, sizeof(EditBookMessage));

  LibClient_sendData((const uint8_t *)buffer, sizeof(LIB_HDR) + libHdr.bodyLength);
}

void LibBoard_printRentBook(void)
{
  LIB_HDR libHdr;
  RentBookInfo rentBookInfo;
  memset(&libHdr, 0, sizeof(LIB_HDR));
  memset(&rentBookInfo, 0, sizeof(RentBookInfo));
  char buffer[sizeof(LIB_HDR) + sizeof(RentBookInfo)] = {0};

  puts(" ---- 대출정보 입력 ----- ");
  printf("책ID : ");
  scanf("%29s", rentBookInfo.bookID);

  printf("회원 ID : ");
  scanf("%29s", rentBookInfo.memberID);

  libHdr.msgID = RENT_RECV_MSG;
  libHdr.bodyLength = sizeof(RentBookInfo);

  memcpy(&buffer[0], &libHdr, sizeof(LIB_HDR));
  memcpy(&buffer[sizeof(LIB_HDR)], &rentBookInfo, sizeof(RentBookInfo));

  LibClient_sendData((const uint8_t *)buffer, sizeof(LIB_HDR) + libHdr.bodyLength);
}

void LibBoard_printReturnBook(void)
{
  LIB_HDR libHdr;
  memset(&libHdr, 0, sizeof(LIB_HDR));
  char token[BOOK_FIELD_LENGTH] = {0};
  char buffer[sizeof(LIB_HDR) + sizeof(token)] = {0};

  puts(" ---- 반납정보 입력 ----- ");
  printf("책ID : ");
  scanf("%29s", token);

  libHdr.msgID = RETURN_RECV_MSG;
  libHdr.bodyLength = sizeof(token);

  memcpy(&buffer[0], &libHdr, sizeof(LIB_HDR));
  memcpy(&buffer[sizeof(LIB_HDR)], token, sizeof(token));

  LibClient_sendData((const uint8_t *)buffer, sizeof(LIB_HDR) + libHdr.bodyLength);
}

void LibBoard_printMybookList(void)
{
  LIB_HDR libHdr;
  memset(&libHdr, 0, sizeof(LIB_HDR));
  char token[BOOK_FIELD_LENGTH] = {0};
  char buffer[sizeof(LIB_HDR) + sizeof(Book)] = {0};

  puts(" ---- 나의 대출목록 불러오기 ----- ");
  printf("회원ID : ");
  scanf("%29s", token);

  libHdr.msgID = MY_BOOK_LIST_RECV_MSG;
  libHdr.bodyLength = sizeof(token);

  memcpy(&buffer[0], &libHdr, sizeof(LIB_HDR));
  memcpy(&buffer[sizeof(LIB_HDR)], token, sizeof(token));

  LibClient_sendData((const uint8_t *)buffer, sizeof(LIB_HDR) + libHdr.bodyLength);
}