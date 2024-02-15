#include "LibSession.h"
#include "LibMain.h"

void LibClient_Logout(Session *session);
void *LibClient_TimerThread(void *arg);
void LibClient_CreateSession(void);
void LibSession_RecvThreadSetup(void);

Session session;

void LibClient_CreateSession(void)
{
  memset(&session, 0, sizeof(Session));
  session.adminState = GENERAL_USER;
  session.lastActiveTime = time(NULL);
}

void LibClient_Logout(Session *session)
{
  memset(session, 0, sizeof(Session));
}

void LibClient_UpdateActivityTime(void)
{
  session.lastActiveTime = time(NULL);
}

void *LibClient_TimerThread(void *arg)
{
  Session *session = (Session *)arg;
  while (1)
  {
    sleep(1);
    if (time(NULL) - session->lastActiveTime > 60)
    {
      LibClient_Logout(session);
      break;
    }
  }
  pthread_exit(NULL);
}

void LibSession_RecvThreadSetup(void)
{
  static pthread_t sessionTimerThread = 0;
  if (pthread_create(&sessionTimerThread, NULL, LibClient_TimerThread, &session) != 0)
  {
    perror("pthread create error");
    exit(EXIT_FAILURE);
  }
}

void LibSession_Init(void)
{
  LibClient_CreateSession();
  LibSession_RecvThreadSetup();
}