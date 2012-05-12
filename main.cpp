#include <QtGui/QApplication>
#include "kuechenlichtcontrol.h"
#include <QColorDialog>
#include <QMutex>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Dgram.h>
#include <ace/Time_Value.h>
#include <udpkuechenlicht.h>
#include <QThread>
#include <QObject>
#include <messagehandler.h>
#include <QTimer>

ACE_INET_Addr           kBCAddr (KUECHENLICHT_UDP_BC_PORT);
ACE_INET_Addr           kSendAddr (KUECHENLICHT_UDP_CMD_PORT);
ACE_INET_Addr           mBCAddress;

volatile ACE_SOCK_Dgram_Bcast*   pBCSocket;
volatile ACE_SOCK_Dgram*         pSendSocket;

volatile std::map<std::string, kuechenLicht_rsp_set>    mLichterMap;
ACE_Time_Value          timeout (1);

std::string             mBCAddressString="192.168.1.128";

QMutex mutexSendSock,mutexBCSock,mutexLichterMap;

volatile bool alive=true;


int main(int argc, char *argv[])
{
    pBCSocket =new(ACE_SOCK_Dgram_Bcast(kCmdAddr));
    pSendSocket =new(ACE_SOCK_Dgram(kSendAddr));
    pSendSocket =new(ACE_SOCK_Dgram(kSendAddr));

    pSendSocket->open(kSendAddr);
    mBCAddress.set(KUECHENLICHT_UDP_BC_PORT,mBCAddressString.c_str());

    QApplication a(argc, argv);
    KuechenLichtControl w;

    QThread* pthread=new QThread;
    MessageHandler *pmessagehandler = new MessageHandler;

    QTimer* timerMessagePolling = new QTimer(this);
    QObject::connect(timerMessagePolling, SIGNAL(timeout()), pmessagehandler, SLOT(handleMessage()));


    pmessagehandler->moveToThread(pthread);

    thread->start();
    timerMessagePolling->start(500);
    w.setup();
    w.show();



    return a.exec(); //Benutzerinteraktion startet
}
