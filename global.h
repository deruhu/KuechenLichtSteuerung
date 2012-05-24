#ifndef GLOBAL_H
#define GLOBAL_H

#include <map>
#include <QMutex>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Dgram.h>
#include <ace/SOCK_Dgram_Bcast.h>
#include <udpkuechenlicht.h>
#include <QQueue>

extern QMutex mutexSendQueue,mutexHandlerCondSock,mutexBCQueue,mutexLichterMap;

extern volatile bool alive;

extern std::map<std::string, kuechenLicht_rsp_set>    mLichterMap;

typedef struct{
   char             message[500];
   size_t           size;
}bcMessageContainer;

typedef struct{
   char             message[500];
   ACE_INET_Addr    modulAddr;
   size_t           size;
}cmdMessageContainer;

extern QQueue<cmdMessageContainer> commandMessageQueue;

extern QQueue<bcMessageContainer> broadcastMessageQueue;


#endif // GLOBAL_H
