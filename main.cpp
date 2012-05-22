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

 ACE_SOCK_Dgram_Bcast   *pBCSocket;
 ACE_SOCK_Dgram         *pSendSocket;

std::map<std::string, kuechenLicht_rsp_set>    mLichterMap;
ACE_Time_Value          sendtimeout (0,200000);

std::string             mBCAddressString="192.168.1.128";

QMutex mutexSendSock,mutexBCSock,mutexLichterMap;

volatile bool alive=true;



int main(int argc, char *argv[])
{
    pBCSocket =new ACE_SOCK_Dgram_Bcast(kBCAddr);
    pSendSocket =new ACE_SOCK_Dgram(kSendAddr);

    pSendSocket->open(kSendAddr);
    mBCAddress.set(KUECHENLICHT_UDP_BC_PORT,mBCAddressString.c_str());

    QApplication a(argc, argv);
    KuechenLichtControl* w = new KuechenLichtControl;

    QThread* pthread=new QThread();
    QThread* timerthread=new QThread();

    MessageHandler *pmessagehandler = new MessageHandler;

    QTimer *timerMessagePolling=new QTimer(0);

    timerMessagePolling->setInterval(500);

    timerMessagePolling->moveToThread(timerthread);

    pmessagehandler->moveToThread(pthread);



    QObject::connect(timerMessagePolling, SIGNAL(timeout()), pmessagehandler, SLOT(handleMessage()));

    QObject::connect(pmessagehandler, SIGNAL(signalNewModule(std::string)),w,SLOT(newModuleSlot(std::string)));

    QObject::connect(pmessagehandler,SIGNAL(signalStatusUpdate()),w,SLOT(changedStatusSlot()));

    QObject::connect(w,SIGNAL(sendCmdMessage(kuechenLichtHeader*,ACE_INET_Addr*)),pmessagehandler,SLOT(sendCommand(kuechenLichtHeader*,ACE_INET_Addr*)));

    QObject::connect(w,SIGNAL(sendBCMessage(kuechenLichtHeader*)),pmessagehandler,SLOT(sendBCMessage(kuechenLichtHeader*)));

    pthread->start();

    w->setup();
    w->show();



    return a.exec(); //Benutzerinteraktion startet
}
