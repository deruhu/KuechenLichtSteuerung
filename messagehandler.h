#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QThread>
#include <QMutex>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Dgram.h>
#include <ace/SOCK_Dgram_Bcast.h>
#include <udpkuechenlicht.h>

class MessageHandler : public QObject
{
    Q_OBJECT
public:

    MessageHandler();//QObject *parent = 0

public slots:

    void handleMessage();

private slots:

    void handleSetResponse(kuechenLicht_rsp_set*,ACE_INET_Addr*);
    void handleCAYFResponse(kuechenLicht_rsp_cayf*, ACE_INET_Addr*);
    void sendCommand(kuechenLichtHeader*,ACE_INET_Addr*);
    void sendBCMessage(kuechenLichtHeader*);

signals:
   void signalNewModule(std::string);
   void signalStatusUpdate(void);

private:
   ACE_INET_Addr           kBCAddr;
   ACE_INET_Addr           kSendAddr;
   ACE_INET_Addr           mBCAddress;

   ACE_SOCK_Dgram_Bcast   *pBCSocket;
   ACE_SOCK_Dgram         *pSendSocket;

};
#endif // MESSAGEHANDLER_H
