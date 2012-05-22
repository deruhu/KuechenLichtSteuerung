#ifndef GLOBAL_H
#define GLOBAL_H

#include <map>
#include <QMutex>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Dgram.h>
#include <ace/SOCK_Dgram_Bcast.h>

extern ACE_INET_Addr           kBCAddr;
extern ACE_INET_Addr           kSendAddr;
extern ACE_INET_Addr           mBCAddress;

extern ACE_SOCK_Dgram_Bcast    *pBCSocket;
extern ACE_SOCK_Dgram          *pSendSocket;

extern std::map<std::string, kuechenLicht_rsp_set>    mLichterMap;
extern ACE_Time_Value          sendtimeout;

extern std::string             mBCAddressString;

extern QMutex mutexSendSock,mutexHandlerCondSock,mutexBCSock,mutexLichterMap;

extern volatile bool alive;

#endif // GLOBAL_H
