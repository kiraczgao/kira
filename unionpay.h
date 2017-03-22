#ifndef UNIONPAY_H
#define UNIONPAY_H

#include <QObject>
#include <QString>
#include "cuptrans.h"
#include <QTimer>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include "unionpaystructure.h"
#include "datastructure.h"

//class unionPay : public QObject, public CUPTrans, public QThread
class unionPay : public QThread
{
    Q_OBJECT

public:
    explicit unionPay();
    void initUnionPayConfig();
    void recordTermMk(QString stermMk);

signals:
    void unionPayA1Talk();
    void unionPayA2Talk();
    void unionPayTradeSuccess();
    void unionPayTradeFailed();

public slots:
    void onTimerOut();
    void beatTimerOut();
    int processA2Talk(unionPayInfo_t* pTag,cardreceiveA2 unionPayProcA2, int tickets);

protected:
    void run();

public:
    void stop();

public:
    QString serverIpstr;
    QString serverPortstr;
    QString merchIdstr;
    QString termIdstr;
    QString batchIdstr;
    QString termMkstr;
    CUPTrans myCUPTrans;
    QTimer *myTimer;
    QTimer *unionPayBeatTimer;

public:
    void getUnionPayTradeRet(int* pret);
};

#endif // UNIONPAY_H
