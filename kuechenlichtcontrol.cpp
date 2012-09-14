#include "kuechenlichtcontrol.h"
#include "ui_kuechenlichtcontrol.h"
#include <sstream>
#include <string>
#include <Qt>
#include <QColor>
#include <QColorDialog>
#include <QMessageBox>
#include <QComboBox>
#include <ace/INET_Addr.h>
#include <udpkuechenlicht.h>
#include <QMutex>
#include "global.h"
#include <QQueue>
#include <map>
#include <iostream>
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
}

void KuechenLichtControl::on_sendButton_clicked()
{
    QString IPString;
    IPString=ui->lineEditIP->text();
    std::cout<<IPString.toStdString()<<std::endl;
    ACE_INET_Addr remote(static_cast<unsigned short>(KUECHENLICHT_UDP_CMD_PORT), IPString.toStdString().c_str());


    ledrgb.rotString=ui->lineEditRot->text();
    ledrgb.gruenString=ui->lineEditGruen->text();
    ledrgb.blauString=ui->lineEditBlau->text();
    ledFarbe.setRed(ledrgb.rotString.toInt());
    ledFarbe.setGreen(ledrgb.gruenString.toInt());
    ledFarbe.setBlue(ledrgb.blauString.toInt());
    kSenderLEDStatus.rot=(unsigned char)ledFarbe.red();
    kSenderLEDStatus.gruen=(unsigned char)ledFarbe.green();
    kSenderLEDStatus.blau=(unsigned char)ledFarbe.blue();

    kuechenLicht_cmd_set* commandSet=new kuechenLicht_cmd_set;

    commandSet->kHeader.magic = KUECHENLICHT_MAGIC;
    commandSet->kHeader.modulAddress =  remote.get_ip_address();
    commandSet->kHeader.senderAddress = 0;
    commandSet->kHeader.messageID = KUECHENLICHT_ID_CMD_SET;
    commandSet->kHeader.messageLength = (ACE_UINT16)sizeof(kuechenLicht_cmd_set);

    commandSet->kLEDStatus.rot = kSenderLEDStatus.rot;
    commandSet->kLEDStatus.gruen = kSenderLEDStatus.gruen;
    commandSet->kLEDStatus.blau = kSenderLEDStatus.blau;

    commandSet->checksum = kSenderLEDStatus.rot ^  kSenderLEDStatus.gruen ^  kSenderLEDStatus.blau;

    int kDurchlaeufe=0;
    bool erfolg=false;
    std::map<std::string, kuechenLicht_rsp_set>::iterator it;
    while((kDurchlaeufe<15) && (!erfolg))
    {
        cmdMessageContainer box;
        box.modulAddr=remote;
        box.size=sizeof(kuechenLicht_cmd_set);
        memcpy(box.message ,commandSet,sizeof(kuechenLicht_cmd_set));

        mutexSendQueue.lock();
        commandMessageQueue.enqueue(box);
        mutexSendQueue.unlock();

        emit sendCmdMessages();

        mutexLichterMap.lock();
        it=mLichterMap.find(remote.get_host_addr());
        erfolg=compareLEDStatus(it->second.kLEDStatus,commandSet->kLEDStatus);
        mutexLichterMap.unlock();

        ++kDurchlaeufe;
    }
    delete commandSet;
}

bool KuechenLichtControl::compareLEDStatus(kuechenLichtLEDStatus LEDStat1,kuechenLichtLEDStatus LEDStat2)
{
    if((LEDStat1.blau) != (LEDStat2.blau))
        return false;

    if((LEDStat1.gruen) != (LEDStat2.gruen))
        return false;

    if((LEDStat1.rot) != (LEDStat2.rot))
        return false;

    return true;
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
    kuechenLicht_cmd_come_all_ye_faithful* commandCome= new kuechenLicht_cmd_come_all_ye_faithful;


    commandCome->kHeader.magic = KUECHENLICHT_MAGIC;
    commandCome->kHeader.modulAddress =  0;
    commandCome->kHeader.senderAddress = 0;
    commandCome->kHeader.messageID = KUECHENLICHT_ID_CMD_SET;
    commandCome->kHeader.messageLength = (ACE_UINT16)sizeof(kuechenLicht_cmd_come_all_ye_faithful);

    commandCome->kNumberofCalls = 0;

    unsigned char i=0;

    while(i<15)
    {
        bcMessageContainer box;
        memcpy(box.message ,commandCome,sizeof(kuechenLicht_cmd_set));
        box.size=sizeof(kuechenLicht_cmd_set);

        mutexBCQueue.lock();
        broadcastMessageQueue.enqueue(box);
        mutexBCQueue.unlock();

        emit sendBCMessages();
        commandCome->kNumberofCalls=i;

        i++;
    }
    delete commandCome;
}


void KuechenLichtControl::newModuleSlot(std::string aIPString)
{
    ui->foundModulesComboBox->addItem(QString (aIPString.c_str()));
}

void KuechenLichtControl::changedStatusSlot()
{

}

void KuechenLichtControl::on_foundModulesComboBox_activated(const QString &arg1)
{

}
