#ifndef BUSLINEWIDGET_H
#define BUSLINEWIDGET_H

#include <QWidget>
#include <QString>
#include "datastructure.h"

namespace Ui {
class buslineWidget;
}

class buslineWidget : public QWidget
{
    Q_OBJECT

public:
    explicit buslineWidget(QWidget *parent = 0);
    ~buslineWidget();

private:
    Ui::buslineWidget *ui;

public:
    void setBusline(QString sline);
    void getBusline(QString &gline) { gline = busline; }
    void setBusticket(QString sticket);
    void getBusticket(QString &gticket) { gticket = ticket; }
    void keyboardLeftpress();
    void keyboardRightpress();
    void keyboardShortpress();
    void initLabelState();

signals:
    void checkConfirm();

public:
    QString busline;
    QString ticket;
    lineTicketLabelState myIDLabel;
    QString buslineArray[8];
    QString ticketArray[5];
};

#endif // BUSLINEWIDGET_H
