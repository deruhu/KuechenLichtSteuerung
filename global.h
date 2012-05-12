#ifndef GLOBAL_H
#define GLOBAL_H

#include <map>
#include <QMutex>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Dgram.h>
#include <ace/SOCK_Dgram_Bcast.h>

extern ACE_INET_Addr           kBCAddr;
extern ACE_INET_Addr           kSendAddr;
extern ACE_INET_Addr           kSendAddr;
extern ACE_INET_Addr           mBCAddress;

extern volatile ACE_SOCK_Dgram_Bcast*   pBCSocket;
extern volatile ACE_SOCK_Dgram*         pSendSocket;

extern volatile std::map<std::string, kuechenLicht_rsp_set>    mLichterMap;
extern ACE_Time_Value          timeout;

extern std::string             mBCAddressString;

extern QMutex mutexSendSock,mutexHandlerCondSock,mutexBCSock,mutexLichterMap;

extern volatile bool alive;

#endif // GLOBAL_H
