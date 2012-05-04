#ifndef KUECHENLICHTCONTROL_H
#define KUECHENLICHTCONTROL_H

#include <iostream>
#include <string>
#include <stdlib.h>
#include <QWidget>
#include <QColor>
#include <QColorDialog>
#include <ace/Time_Value.h>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Dgram.h>
#include <ace/SOCK_Dgram_Bcast.h>
#include <udpkuechenlicht.h>
#include <map>

typedef struct {
    QString rotString;
    QString gruenString;
    QString blauString;
}ledFarben;

namespace Ui {
    class KuechenLichtControl;
}

class KuechenLichtControl : public QWidget
{
    Q_OBJECT

public:
    explicit KuechenLichtControl(QWidget *parent = 0);
    ~KuechenLichtControl();

private slots:
    void on_sendButton_clicked();

    void on_selectColorButton_clicked();

    void on_findModulesPushButton_clicked();

private:
    Ui::KuechenLichtControl *ui;
    ledFarben   ledrgb;
    QColor      ledFarbe;
    QColorDialog colorDialog;
    kuechenLichtLEDStatus kSenderLEDStatus;
    ACE_INET_Addr mBCAddress;
    ACE_Time_Value timeout;
    std::string      mBCAddressString;
    ACE_SOCK_Dgram_Bcast* pBCSocket;
    ACE_SOCK_Dgram* pRecSocket;
    ACE_SOCK_Dgram* pSendSocket;
    std::map<ACE_UINT32, kuechenLicht_rsp_set>    mLichterMap;


    void findModules(void);
    void handleMessage(ssize_t recv_cnt,char* buff,bool* erfolg,ACE_INET_Addr* pSollSender, ACE_INET_Addr* pIstSender);
    bool handleSetResponse(kuechenLicht_rsp_set*,ACE_INET_Addr* , ACE_INET_Addr*);
    void setup();
};

#endif // KUECHENLICHTCONTROL_H
