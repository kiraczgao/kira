#ifndef POSWIDGET_H
#define POSWIDGET_H

#include <QWidget>
#include <QString>
#include "scanserialtalk.h"
#include "tradedialog.h"

class QTimer;
namespace Ui {
class posWidget;
}

class posWidget : public QWidget
{
    Q_OBJECT

public:
    explicit posWidget(QWidget *parent = 0);
    ~posWidget();
    void setTickets(QString& stickets);
    void getTickets(QString& gtickets) { gtickets = tickets; }
    void setBusID(const QString& sbusID);
    void getBusID(QString& gbusID) { gbusID = busID; }
    void setBuslineID(const QString& sbuslineID);
    void getBuslineID(QString& gbuslineID) { gbuslineID = buslineID; }
    void setposID(const QString& sposID);
    void getposID(QString& gposID) { gposID = posID; }
    void setdriverID(const QString& sdriverID);
    void getdriverID(QString& gdriverID) { gdriverID = driverID; }

private:
    Ui::posWidget *ui;

public slots:
    void onTimerOut();

public:
    QString tickets;
    QString busID;
    QString buslineID;
    QString posID;
    QString driverID;

public:
    QTimer *myTimer;
    bool isShowPosWidget;
};

#endif // POSWIDGET_H
