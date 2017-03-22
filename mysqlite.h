#ifndef MYSQLITE_H
#define MYSQLITE_H

#include <QObject>
#include <QtSql/QtSql>
#include <stdlib.h>
#include <QMutex>
#include <QMutexLocker>
#include <QMap>
#include "datastructure.h"

class mySqlite : public QObject
{
    Q_OBJECT
public:
    explicit mySqlite(QObject *parent = 0);
    bool bulidSqliteDatabase(const QString dbname);
    bool createSqliteTable(const QString createSql, const QString tablename);
    bool insertConsumeData(dbConsumeInfo_t consumeData);
    bool insertTradeData(scanTradeInfo tradeInfo);
    bool judgeRepeatScan(scanTradeInfo tradeInfo, tradeBalance_t* tradeBalance);
    bool judgeRepeatConsume(scanTradeInfo tradeInfo, tradeBalance_t* tradeBalance);
    bool updataServerAck(QString userID, QString QRgenerationTime);
    bool insertAlipayKey(int keyID, QString keyValue);
    bool updataAlipayKey(int keyID, QString keyValue);
    bool getAlipayKey(QMap<int, QString> &key_map);
    // v2 --- 2017.3.10
    bool insertAlipayCardType(int cardTypeID, QString cardTypeValue);
    bool updataAlipayCardType(int cardTypeID, QString cardTypeValue);
    bool getAlipayCardType(QMap<int, QString> &cardType_map);

    bool insertAlipayBlack(int blackID, QString blackValue);
    bool updataAlipayBlack(int blackID, QString blackValue);
    bool getAlipayBlack(QMap<int, QString> &black_map);
    bool findAlipayBlack(QString blackValue);
    bool clearAlipayBlack();

    //v1
    bool insertAlipayTradeInfo(alipayTradeInfo_t tradeInfo, QString QRcode);
    int  judgeAlipayRepeatScan(alipayTradeInfo_t tradeInfo, QString QRcode);
    bool updataAlipayServerAck(QString userID, QString tradeTime, QString tradeRecord);
    bool scanAlipayTradeRecordInfo(alipayTradeInfo_t &tradeInfo);
    //v2 kira added --- 2017.3.9
    bool insertAlipayTradeInfoV2(alipayTradeInfoV2_t tradeInfo, QString QRcode);
    int  judgeAlipayRepeatScanV2(alipayTradeInfoV2_t tradeInfo, QString QRcode);
    bool updataAlipayServerAckV2(QString userID, QString tradeTime, QString tradeRecord);
    bool scanAlipayTradeRecordInfoV2(alipayTradeInfoV2_t &tradeInfo);

    bool insertAlipayAbnormalQR(QString QRstr, QString datetimeStr, int keyret, int cryptret);
    bool clearAlipayOutTradeInfo();
    bool clearAlipayOutTradeInfoV2();   // V2 -- 2017.3.9
    bool clearAlipayOutAbnormalQR();
    void setAbnormalOuttime(QString outtime) { abnormalOuttime = outtime; }
    void setTradeOuttime(QString outtime) { tradeOuttime = outtime; }

    bool processDriverSign(driveSignNetInfo_t &signNetInfo);
    //bool scanDriverSignInfo(driveSignNetInfo_t &psignNetInfo);
    bool insertDriverSignInfo(driveSignNetInfo_t signNetInfo);
    bool updataDriverSignAck(QString driverID, QString signTime);
    bool scanDriverSignInfo(driveSignNetInfo_t &signNetInfo);
    bool initDriverSignInfo(QString &driverID);
    bool clearDriverSignInfo();
    bool judgeDriverSignRepeat(QString driverID);
    void setDriverSignOuttime(QString outtime) { driverSignOuttime = outtime; }

    //銀聯信息表相關函數
    bool insertUnionpayInfo(unionPayInfo_t& tag);
    bool selectUnionpayInfo(unionPayInfo_t& tag);
    bool updateUnionpayInfoServerAck(QString cardno,QString tradetime);
    bool clearUnionpayInfo();

signals:

public slots:

public:
    dbConsumeInfo_t myConsumeInfo;
    scanTradeInfo tradeInfo;
    QSqlDatabase myDb;
    QMutex sqliteMutex;
    QString abnormalOuttime;
    QString tradeOuttime;
    QString driverSignOuttime;
    int tradeInfoIDRecord;
    int tradeInfoIDRecordV2;   // v2 --- 2017.3.9
    int driverSignIDRecord;
};

#endif // MYSQLITE_H
