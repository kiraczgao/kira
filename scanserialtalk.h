#ifndef SCANSERIALTALK_H
#define SCANSERIALTALK_H

#include <QWidget>
#include <QSocketNotifier>
#include <QMessageBox>
#include <stdlib.h>
#include <unistd.h>             // linux
#include <fcntl.h>              // linux
#include <errno.h>              // linux
#include "serialdata.h"
#include "datastructure.h"
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#ifndef Q_MOC_RUN
#include "../boost_1_58_0/boost_include.h"
#endif
#include "setconfig.h"

class QTimer;

//class scanSerialTalk : public QWidget
class scanSerialTalk : public QThread
{
    Q_OBJECT
public:
    //explicit scanSerialTalk(QWidget *parent = 0);
    explicit scanSerialTalk();
    void setTickets(const QString& stickets) { tickets = stickets; }
    void getTickets(QString& gtickets) { gtickets = tickets; }
    void setPosID(const QString& sposID) { posID = sposID; }
    void getPosID(QString& gposID) { gposID = posID; }
    void tradeGledCtrl(int flag);
    void tradeRledCtrl(int flag);
    void tradeScanCtrl(int flag);
    int dsaVerify(unsigned char* signStr, int signStrlen, unsigned char* inputStr, int inputStrlen);
    unsigned char AscToHex(unsigned char aChar);

signals:
    void recvScanInfo();
    void recvDriveSignInfo();
    void recvAlipayScanInfo();
    void recvWeixinScanInfo();
    //初始化设置设备线路、车辆号和票价
    void recvPosParamQRCode(QString,QString,QString);
    void recvPosParamQRCodeEnd();

public slots:
    void readData();
    void onTimerOut();

protected:
    void run();

public:
    void stop();

private:
    bool m_stopFlag;
    QMutex threadMutex;

public:
    QString tickets;
    QString posID;

public:
    bool openSerial();
    void getScanInfo(scanTradeInfo* pTradeInfo);
    void getWeixinScanInfo(weixinScanInfo_t* pWeixinScanInfo);
    void getAlipayScanInfo(alipayScanInfo_t* pAlipayScanInfo);
    void getDriveSignRecordInfo(driveSignRecordInfo_t* pSignInfo);
    SerialData scanSerialData;
    QSocketNotifier* scanMonitor;
    int scanfd;
    scanSerialInfo scanInfo;
    scanTradeInfo tradeInfo;
    weixinScanInfo_t weixinScanInfo;
    alipayScanInfo_t alipayScanInfo;
    driveSignRecordInfo_t driveSignRecordInfo;
    char scanbuf[1024];
    int recvLen;
    int ledGfd;
    int ledRfd;
    int scanctlfd;
    QTimer *myTimer;
    QThread* myThread;
    bool m_bPosParamSettingEnd;
    setConfig m_config;
};

#endif // SCANSERIALTALK_H
