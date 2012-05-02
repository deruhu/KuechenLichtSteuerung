#ifndef KUECHENLICHTCONTROL_H
#define KUECHENLICHTCONTROL_H

#include <QWidget>
#include <QColor>
#include <QColorDialog>
#include <ace/INET_Addr.h>

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

    void findModules(void);
    void handleMessage(ssize_t recv_cnt,char* buff,bool* erfolg,ACE_INET_Addr* pSollSender, ACE_INET_Addr* pIstSender);
};

#endif // KUECHENLICHTCONTROL_H
