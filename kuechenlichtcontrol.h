#ifndef KUECHENLICHTCONTROL_H
#define KUECHENLICHTCONTROL_H

#include <QWidget>

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

private:
    Ui::KuechenLichtControl *ui;
};

#endif // KUECHENLICHTCONTROL_H
