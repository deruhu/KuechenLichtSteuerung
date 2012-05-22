#include "kuechenlichtcontrol.h"
#include "ui_kuechenlichtcontrol.h"
#include <sstream>
#include <string>
#include <Qt>
#include <QColor>
#include <QColorDialog>
#include <QMessageBox>
#include <QComboBox>
#include <ace/SOCK_Dgram.h>
#include <ace/SOCK_Dgram_Bcast.h>
#include <ace/INET_Addr.h>
#include <ace/Time_Value.h>
#include <udpkuechenlicht.h>
#include <QMutex>
#include "global.h"
//#include "/home/deruhu/git/ethersex/protocols/kuechenlichtprotokoll/udpkuechenlicht.h"


KuechenLichtControl::KuechenLichtControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KuechenLichtControl)
{
    ui->setupUi(this);
}

void KuechenLichtControl::setup()
{
    ModulComboListe.clear();
    findModules();
}

KuechenLichtControl::~KuechenLichtControl()
{
    delete ui;
    delete pBCSocket;
    delete pSendSocket;
}

void KuechenLichtControl::on_sendButton_clicked()
{
    QString IPString;
    IPString=ui->lineEditIP->text();
    ACE_INET_Addr remote((u_short)KUECHENLICHT_UDP_TEST_PORT,IPString.toStdString().c_str());


    ledrgb.rotString=ui->lineEditRot->text();
    ledrgb.gruenString=ui->lineEditGruen->text();
    ledrgb.blauString=ui->lineEditBlau->text();
    ledFarbe.setRed(ledrgb.rotString.toInt());
    ledFarbe.setGreen(ledrgb.gruenString.toInt());
    ledFarbe.setBlue(ledrgb.blauString.toInt());
    kSenderLEDStatus.rot=(unsigned char)ledFarbe.red();
    kSenderLEDStatus.gruen=(unsigned char)ledFarbe.green();
    kSenderLEDStatus.blau=(unsigned char)ledFarbe.blue();

    kuechenLicht_cmd_set commandSet={
        {	KUECHENLICHT_MAGIC,
                remote.get_ip_address(),
                0,
                KUECHENLICHT_ID_CMD_SET,
                (ACE_UINT16)sizeof(kuechenLicht_cmd_set)
        },
        {
            kSenderLEDStatus.rot,
            kSenderLEDStatus.gruen,
            kSenderLEDStatus.blau
        },
        kSenderLEDStatus.rot ^  kSenderLEDStatus.gruen ^  kSenderLEDStatus.blau,
    };

    int kDurchlaeufe=0;
    bool erfolg=false;
    while((kDurchlaeufe<15) && (!erfolg))
    {
        mutexSendSock.lock();
        size_t sent_data_length = pSendSocket->send(&commandSet, sizeof (kuechenLicht_cmd_set), remote);
        mutexSendSock.unlock();

        if(sent_data_length==-1)
        {
            QMessageBox::critical(this, tr("Error"),
                                  tr("Konnte die Daten nicht senden!"));
            return;
        }
    }
}

void KuechenLichtControl::on_selectColorButton_clicked()
{
    ledrgb.rotString=ui->lineEditRot->text();
    ledrgb.gruenString=ui->lineEditGruen->text();
    ledrgb.blauString=ui->lineEditBlau->text();
    ledFarbe.setRed(ledrgb.rotString.toInt());
    ledFarbe.setGreen(ledrgb.gruenString.toInt());
    ledFarbe.setBlue(ledrgb.blauString.toInt());
    ledFarbe=colorDialog.getColor(ledFarbe);
    ledrgb.rotString.setNum(ledFarbe.red(),10);
    ledrgb.gruenString.setNum(ledFarbe.green(),10);
    ledrgb.blauString.setNum(ledFarbe.blue(),10);
    ui->lineEditRot->setText(ledrgb.rotString);
    ui->lineEditGruen->setText(ledrgb.gruenString);
    ui->lineEditBlau->setText(ledrgb.blauString);
}

void KuechenLichtControl::on_findModulesPushButton_clicked()
{
    mutexLichterMap.lock();
    mLichterMap.clear();
    mutexLichterMap.unlock();

    ModulComboListe.clear();

    ui->foundModulesComboBox->clear();

    findModules();
}


void KuechenLichtControl::findModules(void)
{
    kuechenLicht_cmd_come_all_ye_faithful commandCome={
        {KUECHENLICHT_MAGIC,
         mBCAddress.get_ip_address(),
         0,
         KUECHENLICHT_ID_CMD_CAYF,
         (uint16_t)sizeof(kuechenLicht_cmd_come_all_ye_faithful)
        },
        0
    };
    unsigned char i=0;

    while(i<15)
    {
        commandCome.kNumberofCalls=i;
        mutexSendSock.lock();
        pBCSocket->send(&commandCome,sizeof(kuechenLicht_cmd_come_all_ye_faithful),mBCAddress);
        mutexSendSock.unlock();
        i++;
    }
}


void KuechenLichtControl::newModuleSlot(std::string aIPString)
{
    ui->foundModulesComboBox->clear();
    ModulComboListe << aIPString.c_str();

}

void KuechenLichtControl::changedStatusSlot()
{

}

void KuechenLichtControl::on_foundModulesComboBox_activated(const QString &arg1)
{

}
