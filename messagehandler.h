#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QThread>
#include <QMutex>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Dgram.h>
#include <udpkuechenlicht.h>

class MessageHandler : public QObject
{
    Q_OBJECT

public slots:
    MessageHandler();

    void run();

    void handleMessage(ACE_INET_Addr* =0);

private slots:

    bool handleSetResponse(kuechenLicht_rsp_set*,ACE_INET_Addr* , ACE_INET_Addr*);
    void handleCAYFResponse(kuechenLicht_rsp_cayf*, ACE_INET_Addr*);
};
#endif // MESSAGEHANDLER_H
