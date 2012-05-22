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
#include <QStringList>

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

    void setup();

private slots:
    void on_sendButton_clicked();

    void on_selectColorButton_clicked();

    void on_findModulesPushButton_clicked();

    void newModuleSlot(std::string);

    void changedStatusSlot(void);

    void on_foundModulesComboBox_activated(const QString &arg1);

private:
    Ui::KuechenLichtControl *ui;
    ledFarben   ledrgb;
    QColor      ledFarbe;
    QColorDialog colorDialog;
    kuechenLichtLEDStatus kSenderLEDStatus;
    QStringList ModulComboListe;



    void findModules(void);

signals:
    void sendBCMessage(kuechenLichtHeader*);
    void sendCmdMessage(kuechenLichtHeader*,ACE_INET_Addr*);

};

#endif // KUECHENLICHTCONTROL_H
