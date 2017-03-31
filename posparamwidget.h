#ifndef POSPARAMWIDGET_H
#define POSPARAMWIDGET_H

#include <QWidget>

namespace Ui {
class posparamwidget;
}

class posparamwidget : public QWidget
{
    Q_OBJECT

public:
    explicit posparamwidget(QWidget *parent = 0);
    ~posparamwidget();

public:
    void setPosID(const QString &strPosID);
    void setBusID(const QString &strBusID);
    void setLineID(const QString &strLineID);
    void setTicket(const QString &strTicket);

private:
    Ui::posparamwidget *ui;
};

#endif // POSPARAMWIDGET_H
