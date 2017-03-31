#include "mysqlite.h"
#include <QDebug>
#include <QDateTime>

mySqlite::mySqlite(QObject *parent) : QObject(parent)
{
    qDebug("kira --- Initial sqlite");
    if(this->bulidSqliteDatabase("/opt/kira/db/scanPos.db"))
    {
        /* Create SQL statement */
        QString sql;
        sql = "CREATE TABLE CONSUMEINFO("  \
                "ID INTEGER PRIMARY KEY    AUTOINCREMENT," \
                "DEVICEID       CHAR(8)    NOT NULL," \
                "USERID         CHAR(16)   NOT NULL," \
                "TRADETYPE      CHAR,"     \
                "CITYCODE       CHAR(6)," \
                "TRADETIME      CHAR(14)," \
                "TICKETPRICE    CHAR(4)," \
                "AMOUNT         CHAR(4)," \
                "LONGITUDE      CHAR(12)," \
                "LATITUDE       CHAR(12)," \
                "SERVERACK      CHAR);";
        QString tablename = "CONSUMEINFO";
        this->createSqliteTable(sql, tablename);

        sql = "CREATE TABLE TRADEINFO("  \
                "ID INTEGER PRIMARY KEY    AUTOINCREMENT," \
                "FRAMEHEAD           CHAR(7)    NOT NULL," \
                "ACCOUNT             CHAR(16)   NOT NULL," \
                "ACCOUNTSTATE        CHAR," \
                "CITYCODE            CHAR(6),"    \
                "WALLETBALANCE       CHAR(6),"    \
                "MONTHBALANCE        CHAR(6),"    \
                "QRGENERRATIONTIME   CHAR(14),"   \
                "QRVALIDTIME         CHAR(14));";
        tablename = "TRADEINFO";
        this->createSqliteTable(sql, tablename);

        // 添加支付宝双离线支付公钥
        sql = "CREATE TABLE ALIPAYKEYINFO("  \
                "ID INTEGER PRIMARY KEY    AUTOINCREMENT," \
                "KEYID               INTEGER," \
                "KEYVALUE            TEXT);";
        tablename = "ALIPAYKEYINFO";
        this->createSqliteTable(sql, tablename);

        // 添加支付宝双离线黑名单
        sql = "CREATE TABLE ALIPAYBLACKINFO("  \
                "ID INTEGER PRIMARY KEY    AUTOINCREMENT," \
                "BLACKID               INTEGER," \
                "BLACKVALUE            TEXT);";
        tablename = "ALIPAYBLACKINFO";
        this->createSqliteTable(sql, tablename);

        // v2 --- 2017.3.10
        sql = "CREATE TABLE ALIPAYCARDTYPE("  \
                "ID INTEGER PRIMARY KEY    AUTOINCREMENT," \
                "CARDTYPEID               INTEGER," \
                "CARDTYPEVALUE            TEXT);";
        tablename = "ALIPAYCARDTYPE";
        this->createSqliteTable(sql, tablename);

        // 添加支付宝双离线扫码异常记录
        sql = "CREATE TABLE ALIPAYABNORMALINFO("  \
                "ID INTEGER PRIMARY KEY    AUTOINCREMENT," \
                "SERVERACK                 CHAR," \
                "KEYGETRET                 INTEGER," \
                "DECRYPTRET                INTEGER," \
                "DATETIME                  CHAR(14)," \
                "QRCODE                    TEXT);";
        tablename = "ALIPAYABNORMALINFO";
        this->createSqliteTable(sql, tablename);

        // 添加支付宝双离线交易记录
        sql = "CREATE TABLE ALIPAYTRADEINFO("  \
                "ID INTEGER PRIMARY KEY    AUTOINCREMENT," \
                "USERIDLEN                 CHAR(4)," \
                "OFFLINERECORDLEN          CHAR(4)," \
                "TRADERECORD               CHAR(4)," \
                "BUSLINE                   CHAR(8)," \
                "BUSID                     CHAR(8)," \
                "POSID                     CHAR(8)," \
                "CITYID                    CHAR(6)," \
                "TRADETIME                 CHAR(14)," \
                "TICKETPRICE               CHAR(4)," \
                "LONGITUDE                 CHAR(12)," \
                "LATITUDE                  CHAR(12)," \
                "DRIVERID                  CHAR(16)," \
                "SERVERACK                 CHAR," \
                "USERID                    TEXT," \
                "RECORD                    TEXT," \
                "QRCODE                    TEXT);";
        tablename = "ALIPAYTRADEINFO";
        this->createSqliteTable(sql, tablename);

        // v2 --- 2017.3.9
        sql = "CREATE TABLE ALIPAYTRADEINFOV2("  \
                "ID INTEGER PRIMARY KEY    AUTOINCREMENT," \
                "USERIDLEN                 CHAR(4)," \
                "OFFLINERECORDLEN          CHAR(4)," \
                "CARDNOLEN                 CHAR(4)," \
                "CARDDATALEN               CHAR(4)," \
                "CARDTYPELEN               CHAR(4)," \
                "TRADERECORD               CHAR(4)," \
                "BUSLINE                   CHAR(8)," \
                "BUSID                     CHAR(8)," \
                "POSID                     CHAR(8)," \
                "CITYID                    CHAR(6)," \
                "TRADETIME                 CHAR(14)," \
                "TICKETPRICE               CHAR(4)," \
                "LONGITUDE                 CHAR(12)," \
                "LATITUDE                  CHAR(12)," \
                "DRIVERID                  CHAR(16)," \
                "SERVERACK                 CHAR," \
                "USERID                    TEXT," \
                "RECORD                    TEXT," \
                "CARDNO                    TEXT," \
                "CARDDATA                  TEXT," \
                "CARDTYPE                  TEXT," \
                "QRCODE                    TEXT);";
        tablename = "ALIPAYTRADEINFOV2";
        this->createSqliteTable(sql, tablename);

        // 添加司机签到记录
#if 1
        sql = "CREATE TABLE DRIVESIGNINFO("  \
                "ID INTEGER PRIMARY KEY    AUTOINCREMENT," \
                "POSID                     CHAR(8),"  \
                "DRIVERID                  CHAR(16)," \
                "SIGNFLAG                  CHAR,    " \
                "SIGNTIME                  CHAR(14)," \
                "BUSLINE                   CHAR(8),"  \
                "BUSID                     CHAR(8),"  \
                "CITYID                    CHAR(6),"  \
                "LONGITUDE                 CHAR(12),"  \
                "LATITUDE                  CHAR(12),"  \
                "SERVERACK                 CHAR,"   \
                "DRIVERTYPE                CHAR);" ;
#endif
#if 0
        sql = "CREATE TABLE DRIVESIGNINFO("  \
                "ID INTEGER PRIMARY KEY    AUTOINCREMENT," \
                "POSID                     CHAR(8),"  \
                "DRIVERID                  CHAR(16)," \
                "SIGNFLAG                  CHAR,    " \
                "SIGNTIME                  CHAR(14)," \
                "BUSLINE                   CHAR(8),"  \
                "BUSID                     CHAR(8),"  \
                "CITYID                    CHAR(6),"  \
                "LONGITUDE                 CHAR(12),"  \
                "LATITUDE                  CHAR(12),"  \
                "SERVERACK                 CHAR);" ;
#endif
        tablename = "DRIVESIGNINFO";
        this->createSqliteTable(sql, tablename);

        // Add unionpay infomation
        sql = "CREATE TABLE UNIONPAYINFO("  \
              "ID INTEGER PRIMARY KEY    AUTOINCREMENT," \
              "SERVERACK                 CHAR," \
              "BUSLINE                   CHAR(8)," \
              "BUSID                     CHAR(8)," \
              "POSID                     CHAR(8)," \
              "CITYID                    CHAR(6)," \
              "DRIVERID                  CHAR(16)," \
              "TRADETIME                 CHAR(14)," \
              "TICKETPRICE               CHAR(4)," \
              "LONGITUDE                 CHAR(12)," \
              "LATITUDE                  CHAR(12)," \
              "CARDNO                    CHAR(20)," \
              "SERIALNO                  CHAR(3)," \
              "VALIDDATE                 CHAR(4)," \
              "TRADETYPE                 CHAR," \
              "TRADENATURE               CHAR," \
              "SETTLEMENTDATE            CHAR(8),"  \
              "CARDCENTERTIME            CHAR(14),"  \
              "TERMINALID                CHAR(8),"  \
              "MERCHANTID                CHAR(15)," \
              "CURRENCYCODE              CHAR(3),"  \
              "CARDCENTERSERIALNO        CHAR(12)," \
              "RETCODE                   CHAR(2)," \
              "AMOUNT                    CHAR(4));";
        tablename = "UNIONPAYINFO";
        this->createSqliteTable(sql, tablename);
    }

    tradeInfoIDRecord = 0;
    tradeInfoIDRecordV2 = 0;
    driverSignIDRecord = 0;
}

bool mySqlite::bulidSqliteDatabase(const QString dbname)
{
    qDebug() << QSqlDatabase::drivers();

    myDb = QSqlDatabase::addDatabase("QSQLITE");
    //myDb = QSqlDatabase::addDatabase("SQLITECIPHER");

    myDb.setDatabaseName(dbname);
    myDb.setUserName("kira");
    myDb.setPassword("kira1122");
    myDb.setConnectOptions("QSQLITE_CREATE_KEY");
    if(!myDb.open())      
    {
        qDebug()<<"bulid sqlite database failed";
        return false;
    }

    return true;
}

bool mySqlite::createSqliteTable(const QString createSql, const QString tablename)
{
    QSqlQuery myQuery(myDb);

    char sql[100];
    QByteArray nametemp = tablename.toLatin1();

    sprintf(sql, "SELECT count(*) FROM sqlite_master WHERE type='table' AND name='%s';", nametemp.data());
    qDebug(sql);
    myQuery.exec(QString(sql));

    if(myQuery.next())
    {
        if(myQuery.value(0).toInt() == 0)
        {
            if(!myQuery.exec(createSql))
            {
                qDebug()<<"create sqlite table failed";
                qDebug()<<myQuery.lastError().text();
                qDebug()<<myQuery.lastQuery();
                return false;
            }
        }
        else
            qDebug()<<"sqlite table is exit";
    }
    else
        qDebug()<<"create sqlite table success";
    return true;
}

bool mySqlite::insertConsumeData(dbConsumeInfo_t consumeData)
{
    QMutexLocker locker(&sqliteMutex);

    QString deviceIDStr = QByteArray::fromRawData((char*)consumeData.deviceID, sizeof(consumeData.deviceID));
    QString userIDStr = QByteArray::fromRawData((char*)consumeData.userID, sizeof(consumeData.userID));
    QString citycodeStr = QByteArray::fromRawData((char*)consumeData.citycode, sizeof(consumeData.citycode));
    QString tradeTimeStr = QByteArray::fromRawData((char*)consumeData.tradeTime, sizeof(consumeData.tradeTime));
    //QString tradeTypeStr = QByteArray::fromRawData((char*)consumeData.tradeType, sizeof(consumeData.tradeType));
    QString ticketPriceStr = QByteArray::fromRawData((char*)consumeData.ticketPrice, sizeof(consumeData.ticketPrice));
    QString amountStr = QByteArray::fromRawData((char*)consumeData.amount, sizeof(consumeData.amount));
    QString longitudeStr = QByteArray::fromRawData((char*)consumeData.longitude, sizeof(consumeData.longitude));
    QString latitudeStr = QByteArray::fromRawData((char*)consumeData.latitude, sizeof(consumeData.latitude));
    QSqlQuery myQuery(myDb);
    myQuery.prepare("INSERT INTO CONSUMEINFO (DEVICEID,USERID,TRADETYPE,CITYCODE,TRADETIME,TICKETPRICE,AMOUNT,LONGITUDE,LATITUDE, SERVERACK)"
                    "VALUES (:DEVICEID, :USERID, :TRADETYPE, :CITYCODE, :TRADETIME, :TICKETPRICE, :AMOUNT, :LONGITUDE, :LATITUDE, :SERVERACK);");
    myQuery.bindValue(":DEVICEID", deviceIDStr);
    myQuery.bindValue(":USERID", userIDStr);
    myQuery.bindValue(":TRADETYPE", consumeData.tradeType);
    myQuery.bindValue(":CITYCODE", citycodeStr);
    myQuery.bindValue(":TRADETIME", tradeTimeStr);
    myQuery.bindValue(":TICKETPRICE", ticketPriceStr);
    myQuery.bindValue(":AMOUNT", amountStr);
    myQuery.bindValue(":LONGITUDE", longitudeStr);
    myQuery.bindValue(":LATITUDE", latitudeStr);
    myQuery.bindValue(":SERVERACK", 0);
    if(!myQuery.exec())
    {
        qDebug()<<"insert consume sqlite data failed";
        return false;
    }
    else
        return true;
}


bool mySqlite::judgeRepeatConsume(scanTradeInfo tradeInfo, tradeBalance_t* tradeBalance)
{
    //QMutexLocker locker(&sqliteMutex);
    QSqlQuery myQuery(myDb);//以下执行相关SQL语句

    QString accountStr = QByteArray::fromRawData((char*)tradeInfo.account, sizeof(tradeInfo.account));

#if 0
    QString dateTime = QByteArray::fromRawData((char*)tradeInfo.QRgenerationTime, sizeof(tradeInfo.QRgenerationTime));
    double dDateTime = dateTime.toDouble() - MAX_REPEATTIME;
    QString sDateTime = QString::number((qulonglong)dDateTime, 10);
#endif
    QDateTime DateTime = QDateTime::currentDateTime().addSecs((qint64)-MAX_REPEATTIME);
    QString sDateTime = DateTime.toString("yyyyMMddhhmmss");

    QString mySql = QString("select id, TRADETYPE from CONSUMEINFO where TRADETIME>='%1' AND USERID='%2';")
                    .arg(sDateTime).arg(accountStr);
    qDebug(qPrintable(mySql));
    myQuery.exec(mySql);
    bool selectRet = false;
    while(myQuery.next())//query.next()指向查找到的第一条记录，然后每次后移一条记录
    {
        int ele0=myQuery.value(0).toInt();    //query.value(0)是id的值，将其转换为int型
        int ele1=myQuery.value(1).toInt();
        tradeBalance->monthRepeat = false;
        tradeBalance->walletRepeat = false;
        if(ele1 == MONTHTRADE)
        {
            selectRet = true;           
            qDebug("kira --- have month repeat consume");
            qDebug()<<ele0<<ele1;//输出两个值
            tradeBalance->monthRepeat = true;
            //break;
        }
        else
        {
            qDebug("kira --- have wallet repeat consume");
            qDebug()<<ele0<<ele1;//输出两个值
            tradeBalance->walletRepeat = true;
        }
    }
    myQuery.exec(QObject::tr("drop CONSUMEINFO"));
    return selectRet;
}

bool mySqlite::insertTradeData(scanTradeInfo tradeInfo)
{
    QMutexLocker locker(&sqliteMutex);

    QString frameheadStr = QByteArray::fromRawData((char*)tradeInfo.framehead, sizeof(tradeInfo.framehead));
    QString accountStr = QByteArray::fromRawData((char*)tradeInfo.account, sizeof(tradeInfo.account));
    QString citycode = QByteArray::fromRawData((char*)tradeInfo.citycode, sizeof(tradeInfo.citycode));
    QString walletBalanceStr = QByteArray::fromRawData((char*)tradeInfo.walletBalance, sizeof(tradeInfo.walletBalance));
    QString monthBalanceStr = QByteArray::fromRawData((char*)tradeInfo.monthBalance, sizeof(tradeInfo.monthBalance));
    QString QRgenerationTimeStr = QByteArray::fromRawData((char*)tradeInfo.QRgenerationTime, sizeof(tradeInfo.QRgenerationTime));
    QString QRvalidTimeStr = QByteArray::fromRawData((char*)tradeInfo.QRvalidTime, sizeof(tradeInfo.QRvalidTime));
    QSqlQuery myQuery(myDb);
    myQuery.prepare("INSERT INTO TRADEINFO (FRAMEHEAD,ACCOUNT,ACCOUNTSTATE,CITYCODE,WALLETBALANCE,MONTHBALANCE,QRGENERRATIONTIME,QRVALIDTIME)"
                    "VALUES (:FRAMEHEAD, :ACCOUNT, :ACCOUNTSTATE, :CITYCODE, :WALLETBALANCE, :MONTHBALANCE, :QRGENERRATIONTIME, :QRVALIDTIME);");
    myQuery.bindValue(":FRAMEHEAD", frameheadStr);
    myQuery.bindValue(":ACCOUNT", accountStr);
    myQuery.bindValue(":ACCOUNTSTATE", tradeInfo.accountState);
    myQuery.bindValue(":CITYCODE", citycode);
    myQuery.bindValue(":WALLETBALANCE", walletBalanceStr);
    myQuery.bindValue(":MONTHBALANCE", monthBalanceStr);
    myQuery.bindValue(":QRGENERRATIONTIME", QRgenerationTimeStr);
    myQuery.bindValue(":QRVALIDTIME", QRvalidTimeStr);
    if(!myQuery.exec())
    {
        qDebug()<<"insert trade sqlite data failed";
        return false;
    }
    else
        return true;
}

bool mySqlite::judgeRepeatScan(scanTradeInfo tradeInfo, tradeBalance_t* tradeBalance)
{
    //QMutexLocker locker(&sqliteMutex);
    QSqlQuery myQuery(myDb);//以下执行相关SQL语句

    QString accountStr = QByteArray::fromRawData((char*)tradeInfo.account, sizeof(tradeInfo.account));
    QString QRgenerationTimeStr = QByteArray::fromRawData((char*)tradeInfo.QRgenerationTime, sizeof(tradeInfo.QRgenerationTime));

#if 0
    double dDateTime = QRgenerationTimeStr.toDouble() - MAX_REPEATTIME;
    QString sDateTime = QString::number((qulonglong)dDateTime, 10);
#endif

    QDateTime DateTime = QDateTime::currentDateTime().addSecs((qint64)-MAX_REPEATTIME);
    QString sDateTime = DateTime.toString("yyyyMMddhhmmss");

    QString mySql= QString("select id from TRADEINFO where QRGENERRATIONTIME>='%1' AND ACCOUNT='%2' AND QRGENERRATIONTIME='%3';")
                    .arg(sDateTime).arg(accountStr).arg(QRgenerationTimeStr);
    qDebug(qPrintable(mySql));
    myQuery.exec(mySql);
    bool selectRet = false;
    int ele0, ele1;
    while(myQuery.next())//query.next()指向查找到的第一条记录，然后每次后移一条记录
    {
        ele0=myQuery.value(0).toInt();    //query.value(0)是id的值，将其转换为int型
        qDebug("kira --- have repeat scan");
        qDebug()<<ele0;//输出id
        selectRet = true;
        break;
    }
    //return true;

    mySql= QString("select WALLETBALANCE, MONTHBALANCE from TRADEINFO where QRGENERRATIONTIME>='%1' AND ACCOUNT='%2';")
            .arg(sDateTime).arg(accountStr);
    qDebug(qPrintable(mySql));
    myQuery.exec(mySql);
    //tradeBalance->repeat = false;
    while(myQuery.next())//query.next()指向查找到的第一条记录，然后每次后移一条记录
    {
        ele0=myQuery.value(0).toInt();
        ele1=myQuery.value(1).toInt();
        qDebug()<<ele0<<ele1;//输出两个值
        tradeBalance->month = ele0;
        tradeBalance->wallet = ele1;
        //tradeBalance->repeat = true;
    }

    myQuery.exec(QObject::tr("drop TRADEINFO"));
    return selectRet;
    //return false;
}

bool mySqlite::updataServerAck(QString userID, QString QRgenerationTime)
{
    QMutexLocker locker(&sqliteMutex);

    QSqlQuery myQuery(myDb);
    QString mySql = QString("UPDATE CONSUMEINFO SET SERVERACK = 1 WHERE USERID='%1' AND TRADETIME='%2';")
                    .arg(userID).arg(QRgenerationTime);
    if(!myQuery.exec(mySql))
    {
        qDebug()<<"updata consume serverAck sqlite data failed";
        return false;
    }
    else
        return true;
}

// Alipay 双离线
bool mySqlite::insertAlipayBlack(int blackID, QString blackValue)
{
    QMutexLocker locker(&sqliteMutex);

    QSqlQuery myQuery(myDb);
    myQuery.prepare("INSERT INTO ALIPAYBLACKINFO (BLACKID, BLACKVALUE)"
                    "VALUES (:BLACKID, :BLACKVALUE);");
    myQuery.bindValue(":BLACKID", blackID);
    myQuery.bindValue(":BLACKVALUE", blackValue);

    if(!myQuery.exec())
    {
        qDebug()<<"insert Alipay blackInfo sqlite data failed";
        return false;
    }
    else
        return true;
}

bool mySqlite::updataAlipayBlack(int blackID, QString blackValue)
{
    QMutexLocker locker(&sqliteMutex);

    QSqlQuery myQuery(myDb);
    QString mySql = QString("UPDATE ALIPAYBLACKINFO SET BLACKVALUE = '%1' WHERE BLACKID = '%2';")
                    .arg(blackID).arg(blackValue);
    if(!myQuery.exec(mySql))
    {
        qDebug()<<"updata Alipay keyInfo sqlite data failed";
        return false;
    }
    else
        return true;
}

bool mySqlite::getAlipayBlack(QMap<int, QString> &black_map)
{
    QSqlQuery myQuery(myDb);//以下执行相关SQL语句

    QString mySql = QString("select BLACKID, BLACKVALUE from ALIPAYBLACKINFO;");
    qDebug(qPrintable(mySql));
    myQuery.exec(mySql);
    bool selectRet = false;
    while(myQuery.next())//query.next()指向查找到的第一条记录，然后每次后移一条记录
    {
        black_map.insert(myQuery.value(0).toInt(), myQuery.value(1).toString());
        selectRet = true;
    }
    myQuery.exec(QObject::tr("drop ALIPAYBLACKINFO"));
    return selectRet;
}

bool mySqlite::findAlipayBlack(QString blackValue)
{
    QSqlQuery myQuery(myDb); //以下执行相关SQL语句
    QString mySql= QString("select BLACKID from ALIPAYBLACKINFO where BLACKVALUE='%1';")
                    .arg(blackValue);
    qDebug(qPrintable(mySql));
    myQuery.exec(mySql);
    bool findRet = false;
    int ele0;
    while(myQuery.next())
    {
        ele0 = myQuery.value(0).toInt();
        qDebug("kira --- Alipay blackID: ");
        qDebug()<<ele0;
        findRet = true;
        break;
    }

    myQuery.exec(QObject::tr("drop ALIPAYBLACKINFO"));
    return findRet;
}

bool mySqlite::clearAlipayBlack()
{
    QSqlQuery myQuery(myDb); //以下执行相关SQL语句
    QString mySql= QString("delete from ALIPAYBLACKINFO;");
    qDebug(qPrintable(mySql));
    if(!myQuery.exec(mySql))
    {
        qDebug()<<"clear alipay black sqlite data failed";
        return false;
    }
    else
        return true;
}

bool mySqlite::insertAlipayKey(int keyID, QString keyValue)
{
    QMutexLocker locker(&sqliteMutex);

    QSqlQuery myQuery(myDb);
    myQuery.prepare("INSERT INTO ALIPAYKEYINFO (KEYID, KEYVALUE)"
                    "VALUES (:KEYID, :KEYVALUE);");
    myQuery.bindValue(":KEYID", keyID);
    myQuery.bindValue(":KEYVALUE", keyValue);

    if(!myQuery.exec())
    {
        qDebug()<<"insert Alipay keyInfo sqlite data failed";
        return false;
    }
    else
        return true;
}

bool mySqlite::updataAlipayKey(int keyID, QString keyValue)
{
    QMutexLocker locker(&sqliteMutex);

    QSqlQuery myQuery(myDb);
    QString mySql = QString("UPDATE ALIPAYKEYINFO SET KEYVALUE = '%1' WHERE KEYID = '%2';")
                    .arg(keyValue).arg(keyID);
    if(!myQuery.exec(mySql))
    {
        qDebug()<<"updata Alipay keyInfo sqlite data failed";
        return false;
    }
    else
        return true;
}

bool mySqlite::getAlipayKey(QMap<int, QString> &key_map)
{
    QSqlQuery myQuery(myDb);//以下执行相关SQL语句

    QString mySql = QString("select KEYID, KEYVALUE from ALIPAYKEYINFO;");
    qDebug(qPrintable(mySql));
    myQuery.exec(mySql);
    bool selectRet = false;
    while(myQuery.next())//query.next()指向查找到的第一条记录，然后每次后移一条记录
    {
        key_map.insert(myQuery.value(0).toInt(), myQuery.value(1).toString());
        selectRet = true;
    }
    myQuery.exec(QObject::tr("drop ALIPAYKEYINFO"));
    return selectRet;
}

// v2 --- 2017.3.10
bool mySqlite::insertAlipayCardType(int cardTypeID, QString cardTypeValue)
{
    QMutexLocker locker(&sqliteMutex);

    QSqlQuery myQuery(myDb);
    myQuery.prepare("INSERT INTO ALIPAYCARDTYPE (CARDTYPEID, CARDTYPEVALUE)"
                    "VALUES (:CARDTYPEID, :CARDTYPEVALUE);");
    myQuery.bindValue(":CARDTYPEID", cardTypeID);
    myQuery.bindValue(":CARDTYPEVALUE", cardTypeValue);

    if(!myQuery.exec())
    {
        qDebug()<<"insert Alipay cardTypeInfo sqlite data failed";
        return false;
    }
    else
        return true;
}

bool mySqlite::updataAlipayCardType(int cardTypeID, QString cardTypeValue)
{
    QMutexLocker locker(&sqliteMutex);

    QSqlQuery myQuery(myDb);
    QString mySql = QString("UPDATE ALIPAYCARDTYPE SET CARDTYPEVALUE = '%1' WHERE CARDTYPEID = '%2';")
                    .arg(cardTypeValue).arg(cardTypeID);
    if(!myQuery.exec(mySql))
    {
        qDebug()<<"updata Alipay cardTypeInfo sqlite data failed";
        return false;
    }
    else
        return true;
}

bool mySqlite::getAlipayCardType(QMap<int, QString> &cardType_map)
{
    QSqlQuery myQuery(myDb);//以下执行相关SQL语句

    QString mySql = QString("select CARDTYPEID, CARDTYPEVALUE from ALIPAYCARDTYPE;");
    qDebug(qPrintable(mySql));
    myQuery.exec(mySql);
    bool selectRet = false;
    while(myQuery.next())//query.next()指向查找到的第一条记录，然后每次后移一条记录
    {
        cardType_map.insert(myQuery.value(0).toInt(), myQuery.value(1).toString());
        selectRet = true;
    }
    myQuery.exec(QObject::tr("drop ALIPAYCARDTYPE"));
    return selectRet;
}

bool mySqlite::insertAlipayTradeInfo(alipayTradeInfo_t tradeInfo, QString QRcode)
{
    QMutexLocker locker(&sqliteMutex);

    int iuserIDLen;
    int iofflineRecordLen;
    int itradeRecord;
    int iticketPrice;
    memcpy(&iuserIDLen, tradeInfo.offlineHeadInfo.userIDLen, 4);
    memcpy(&iofflineRecordLen, tradeInfo.offlineHeadInfo.offLineRecordLen, 4);
    memcpy(&itradeRecord, tradeInfo.offlineHeadInfo.tradeRecord, 4);
    memcpy(&iticketPrice, tradeInfo.offlineHeadInfo.ticketPrice, 4);
    qDebug("kira --- userIDLen: %d", iuserIDLen);
    qDebug("kira --- offLineRecordLen: %d", iofflineRecordLen);
    qDebug("kira --- tradeRecord: %d", itradeRecord);
    qDebug("kira --- ticketPrice: %d", iticketPrice);

    char cuserIDLen[4];
    cuserIDLen[0] = '0' + iuserIDLen/1000;
    cuserIDLen[1] = '0' + (iuserIDLen%1000)/100;
    cuserIDLen[2] = '0' + (iuserIDLen%100)/10;
    cuserIDLen[3] = '0' + iuserIDLen%10;
    char coffLineRecordLen[4];
    coffLineRecordLen[0] = '0' + iofflineRecordLen/1000;
    coffLineRecordLen[1] = '0' + (iofflineRecordLen%1000)/100;
    coffLineRecordLen[2] = '0' + (iofflineRecordLen%100)/10;
    coffLineRecordLen[3] = '0' + iofflineRecordLen%10;
    char ctradeRecord[4];
    ctradeRecord[0] = '0' + itradeRecord/1000;
    ctradeRecord[1] = '0' + (itradeRecord%1000)/100;
    ctradeRecord[2] = '0' + (itradeRecord%100)/10;
    ctradeRecord[3] = '0' + itradeRecord%10;
    char cticketPrice[4];
    cticketPrice[0] = '0' + iticketPrice/1000;
    cticketPrice[1] = '0' + (iticketPrice%1000)/100;
    cticketPrice[2] = '0' + (iticketPrice%100)/10;
    cticketPrice[3] = '0' + iticketPrice%10;

    QString userIDLenStr = QByteArray::fromRawData((char*)cuserIDLen, sizeof(cuserIDLen));
    QString offLineRecordLenStr = QByteArray::fromRawData((char*)coffLineRecordLen, sizeof(coffLineRecordLen));
    QString tradeRecordStr = QByteArray::fromRawData((char*)ctradeRecord, sizeof(ctradeRecord));
    QString buslineStr = QByteArray::fromRawData((char*)tradeInfo.offlineHeadInfo.busline, sizeof(tradeInfo.offlineHeadInfo.busline));
    QString busIDStr = QByteArray::fromRawData((char*)tradeInfo.offlineHeadInfo.busID, sizeof(tradeInfo.offlineHeadInfo.busID));
    QString posIDStr = QByteArray::fromRawData((char*)tradeInfo.offlineHeadInfo.posID, sizeof(tradeInfo.offlineHeadInfo.posID));
    QString cityIDStr = QByteArray::fromRawData((char*)tradeInfo.offlineHeadInfo.cityID, sizeof(tradeInfo.offlineHeadInfo.cityID));
    QString tradeTimeStr = QByteArray::fromRawData((char*)tradeInfo.offlineHeadInfo.tradeTime, sizeof(tradeInfo.offlineHeadInfo.tradeTime));
    QString ticketPriceStr = QByteArray::fromRawData((char*)cticketPrice, sizeof(cticketPrice));
    QString longitudeStr = QByteArray::fromRawData((char*)tradeInfo.offlineHeadInfo.longitude, sizeof(tradeInfo.offlineHeadInfo.longitude));
    QString latitudeStr = QByteArray::fromRawData((char*)tradeInfo.offlineHeadInfo.latitude, sizeof(tradeInfo.offlineHeadInfo.latitude));
    QString driverIDStr = QByteArray::fromRawData((char*)tradeInfo.offlineHeadInfo.driverID, sizeof(tradeInfo.offlineHeadInfo.driverID));
    QString userIDStr = QByteArray::fromRawData((char*)tradeInfo.userID, iuserIDLen);
    QString recordStr = QByteArray::fromRawData((char*)tradeInfo.record, iofflineRecordLen);

    QSqlQuery myQuery(myDb);
    myQuery.prepare("INSERT INTO ALIPAYTRADEINFO (USERIDLEN,OFFLINERECORDLEN,TRADERECORD,BUSLINE,BUSID,POSID,CITYID,TRADETIME,TICKETPRICE,LONGITUDE,LATITUDE,DRIVERID,SERVERACK,USERID,RECORD,QRCODE)"
                    "VALUES (:USERIDLEN, :OFFLINERECORDLEN, :TRADERECORD, :BUSLINE, :BUSID, :POSID, :CITYID, :TRADETIME, :TICKETPRICE, :LONGITUDE, :LATITUDE, :DRIVERID, :SERVERACK, :USERID, :RECORD, :QRCODE);");

    myQuery.bindValue(":USERIDLEN", userIDLenStr);
    myQuery.bindValue(":OFFLINERECORDLEN", offLineRecordLenStr);
    myQuery.bindValue(":TRADERECORD", tradeRecordStr);
    myQuery.bindValue(":BUSLINE", buslineStr);
    myQuery.bindValue(":BUSID", busIDStr);
    myQuery.bindValue(":POSID", posIDStr);
    myQuery.bindValue(":CITYID", cityIDStr);
    myQuery.bindValue(":TRADETIME", tradeTimeStr);
    myQuery.bindValue(":TICKETPRICE", ticketPriceStr);
    myQuery.bindValue(":LONGITUDE", longitudeStr);
    myQuery.bindValue(":LATITUDE", latitudeStr);
    myQuery.bindValue(":DRIVERID", driverIDStr);
    myQuery.bindValue(":SERVERACK", 0);
    myQuery.bindValue(":USERID", userIDStr);
    myQuery.bindValue(":RECORD", recordStr);
    myQuery.bindValue(":QRCODE", QRcode);
    if(!myQuery.exec())
    {
        qDebug()<<"insert Alipay trade sqlite data failed";
        return false;
    }
    else
        return true;
}

// V2 --- 2017.3.9
bool mySqlite::insertAlipayTradeInfoV2(alipayTradeInfoV2_t tradeInfo, QString QRcode)
{
    QMutexLocker locker(&sqliteMutex);

    if(QRcode.isEmpty()){
        printf("qwy---QRcode is empty\n");
        return false;
    }

    int iuserIDLen;
    int iofflineRecordLen;
    int icardNoLen;
    int icardDataLen;
    int icardTypeLen;
    int itradeRecord;
    int iticketPrice;
    memcpy(&iuserIDLen, tradeInfo.offlineHeadInfo.userIDLen, 4);
    memcpy(&iofflineRecordLen, tradeInfo.offlineHeadInfo.offLineRecordLen, 4);
    memcpy(&icardNoLen, tradeInfo.offlineHeadInfo.cardNoLen, 4);
    memcpy(&icardDataLen, tradeInfo.offlineHeadInfo.cardDataLen, 4);
    memcpy(&icardTypeLen, tradeInfo.offlineHeadInfo.cardTypeLen, 4);
    memcpy(&itradeRecord, tradeInfo.offlineHeadInfo.tradeRecord, 4);
    memcpy(&iticketPrice, tradeInfo.offlineHeadInfo.ticketPrice, 4);
    qDebug("kira --- userIDLen: %d", iuserIDLen);
    qDebug("kira --- offLineRecordLen: %d", iofflineRecordLen);
    qDebug("kira --- tradeRecord: %d", itradeRecord);
    qDebug("kira --- cardNoLen: %d", icardNoLen);
    qDebug("kira --- cardDataLen: %d", icardDataLen);
    qDebug("kira --- cardTypeLen: %d", icardTypeLen);
    qDebug("kira --- ticketPrice: %d", iticketPrice);

    char cuserIDLen[4];
    cuserIDLen[0] = '0' + iuserIDLen/1000;
    cuserIDLen[1] = '0' + (iuserIDLen%1000)/100;
    cuserIDLen[2] = '0' + (iuserIDLen%100)/10;
    cuserIDLen[3] = '0' + iuserIDLen%10;
    char coffLineRecordLen[4];
    coffLineRecordLen[0] = '0' + iofflineRecordLen/1000;
    coffLineRecordLen[1] = '0' + (iofflineRecordLen%1000)/100;
    coffLineRecordLen[2] = '0' + (iofflineRecordLen%100)/10;
    coffLineRecordLen[3] = '0' + iofflineRecordLen%10;
    char ccardNoLen[4];
    ccardNoLen[0] = '0' + icardNoLen/1000;
    ccardNoLen[1] = '0' + (icardNoLen%1000)/100;
    ccardNoLen[2] = '0' + (icardNoLen%100)/10;
    ccardNoLen[3] = '0' + icardNoLen%10;
    char ccardDataLen[4];
    ccardDataLen[0] = '0' + icardDataLen/1000;
    ccardDataLen[1] = '0' + (icardDataLen%1000)/100;
    ccardDataLen[2] = '0' + (icardDataLen%100)/10;
    ccardDataLen[3] = '0' + icardDataLen%10;
    char ccardTypeLen[4];
    ccardTypeLen[0] = '0' + icardTypeLen/1000;
    ccardTypeLen[1] = '0' + (icardTypeLen%1000)/100;
    ccardTypeLen[2] = '0' + (icardTypeLen%100)/10;
    ccardTypeLen[3] = '0' + icardTypeLen%10;
    char ctradeRecord[4];
    ctradeRecord[0] = '0' + itradeRecord/1000;
    ctradeRecord[1] = '0' + (itradeRecord%1000)/100;
    ctradeRecord[2] = '0' + (itradeRecord%100)/10;
    ctradeRecord[3] = '0' + itradeRecord%10;
    char cticketPrice[4];
    cticketPrice[0] = '0' + iticketPrice/1000;
    cticketPrice[1] = '0' + (iticketPrice%1000)/100;
    cticketPrice[2] = '0' + (iticketPrice%100)/10;
    cticketPrice[3] = '0' + iticketPrice%10;

    QString userIDLenStr = QByteArray::fromRawData((char*)cuserIDLen, sizeof(cuserIDLen));
    QString offLineRecordLenStr = QByteArray::fromRawData((char*)coffLineRecordLen, sizeof(coffLineRecordLen));
    QString tradeRecordStr = QByteArray::fromRawData((char*)ctradeRecord, sizeof(ctradeRecord));
    QString cardNoLenStr = QByteArray::fromRawData((char*)ccardNoLen, sizeof(ccardNoLen));
    QString cardDataLenStr = QByteArray::fromRawData((char*)ccardDataLen, sizeof(ccardDataLen));
    QString cardTypeLenStr = QByteArray::fromRawData((char*)ccardTypeLen, sizeof(ccardTypeLen));
    QString buslineStr = QByteArray::fromRawData((char*)tradeInfo.offlineHeadInfo.busline, sizeof(tradeInfo.offlineHeadInfo.busline));
    QString busIDStr = QByteArray::fromRawData((char*)tradeInfo.offlineHeadInfo.busID, sizeof(tradeInfo.offlineHeadInfo.busID));
    QString posIDStr = QByteArray::fromRawData((char*)tradeInfo.offlineHeadInfo.posID, sizeof(tradeInfo.offlineHeadInfo.posID));
    QString cityIDStr = QByteArray::fromRawData((char*)tradeInfo.offlineHeadInfo.cityID, sizeof(tradeInfo.offlineHeadInfo.cityID));
    QString tradeTimeStr = QByteArray::fromRawData((char*)tradeInfo.offlineHeadInfo.tradeTime, sizeof(tradeInfo.offlineHeadInfo.tradeTime));
    QString ticketPriceStr = QByteArray::fromRawData((char*)cticketPrice, sizeof(cticketPrice));
    QString longitudeStr = QByteArray::fromRawData((char*)tradeInfo.offlineHeadInfo.longitude, sizeof(tradeInfo.offlineHeadInfo.longitude));
    QString latitudeStr = QByteArray::fromRawData((char*)tradeInfo.offlineHeadInfo.latitude, sizeof(tradeInfo.offlineHeadInfo.latitude));
    QString driverIDStr = QByteArray::fromRawData((char*)tradeInfo.offlineHeadInfo.driverID, sizeof(tradeInfo.offlineHeadInfo.driverID));
    QString userIDStr = QByteArray::fromRawData((char*)tradeInfo.userID, iuserIDLen);
    QString recordStr = QByteArray::fromRawData((char*)tradeInfo.record, iofflineRecordLen);
    QString cardNoStr = QByteArray::fromRawData((char*)tradeInfo.cardNo, icardNoLen);
    QString cardDataStr = QByteArray::fromRawData((char*)tradeInfo.cardData, icardDataLen);
    QString cardTypeStr = QByteArray::fromRawData((char*)tradeInfo.cardType, icardTypeLen);

    QSqlQuery myQuery(myDb);
    myQuery.prepare("INSERT INTO ALIPAYTRADEINFOV2 (USERIDLEN,OFFLINERECORDLEN,TRADERECORD,CARDNOLEN,CARDDATALEN,CARDTYPELEN,BUSLINE,BUSID,POSID,CITYID,TRADETIME,TICKETPRICE,LONGITUDE,LATITUDE,DRIVERID,SERVERACK,USERID,RECORD,CARDNO,CARDDATA,CARDTYPE,QRCODE)"
                    "VALUES (:USERIDLEN, :OFFLINERECORDLEN, :TRADERECORD, :CARDNOLEN, :CARDDATALEN, :CARDTYPELEN, :BUSLINE, :BUSID, :POSID, :CITYID, :TRADETIME, :TICKETPRICE, :LONGITUDE, :LATITUDE, :DRIVERID, :SERVERACK, :USERID, :RECORD, :CARDNO, :CARDDATA, :CARDTYPE, :QRCODE);");

    myQuery.bindValue(":USERIDLEN", userIDLenStr);
    myQuery.bindValue(":OFFLINERECORDLEN", offLineRecordLenStr);
    myQuery.bindValue(":TRADERECORD", tradeRecordStr);
    myQuery.bindValue(":CARDNOLEN", cardNoLenStr);
    myQuery.bindValue(":CARDDATALEN", cardDataLenStr);
    myQuery.bindValue(":CARDTYPELEN", cardTypeLenStr);
    myQuery.bindValue(":BUSLINE", buslineStr);
    myQuery.bindValue(":BUSID", busIDStr);
    myQuery.bindValue(":POSID", posIDStr);
    myQuery.bindValue(":CITYID", cityIDStr);
    myQuery.bindValue(":TRADETIME", tradeTimeStr);
    myQuery.bindValue(":TICKETPRICE", ticketPriceStr);
    myQuery.bindValue(":LONGITUDE", longitudeStr);
    myQuery.bindValue(":LATITUDE", latitudeStr);
    myQuery.bindValue(":DRIVERID", driverIDStr);
    myQuery.bindValue(":SERVERACK", 0);
    myQuery.bindValue(":USERID", userIDStr);
    myQuery.bindValue(":RECORD", recordStr);
    myQuery.bindValue(":CARDNO", cardNoStr);
    myQuery.bindValue(":CARDDATA", cardDataStr);
    myQuery.bindValue(":CARDTYPE", cardTypeStr);
    myQuery.bindValue(":QRCODE", QRcode);
    if(!myQuery.exec())
    {
        qDebug()<<"insert AlipayV2 trade sqlite data failed";
        return false;
    }
    else
        return true;
}

int mySqlite::judgeAlipayRepeatScan(alipayTradeInfo_t tradeInfo, QString QRcode)
{
    int iuserIDLen;
    memcpy(&iuserIDLen, tradeInfo.offlineHeadInfo.userIDLen, 4);
    qDebug("kira --- userIDLen: %d", iuserIDLen);
    QString userIDStr = QByteArray::fromRawData((char*)tradeInfo.userID, iuserIDLen);

    QSqlQuery myQuery(myDb); //以下执行相关SQL语句

#if 0
    QString tradeTimeStr = QByteArray::fromRawData((char*)tradeInfo.offlineHeadInfo.tradeTime, sizeof(tradeInfo.offlineHeadInfo.tradeTime));
    qDebug(qPrintable(tradeTimeStr));
    double dDateTime = tradeTimeStr.toDouble() - MAX_ALIPAY_REPEATTIME;
    QString sDateTime = QString::number((qulonglong)dDateTime, 10);
#endif

    QDateTime DateTime = QDateTime::currentDateTime().addSecs((qint64)-MAX_ALIPAY_REPEATTIME);
    QString sDateTime = DateTime.toString("yyyyMMddhhmmss");
    qDebug(qPrintable(sDateTime));

    QString mySql= QString("select id from ALIPAYTRADEINFO where TRADETIME>='%1' AND USERID='%2';")
                    .arg(sDateTime).arg(userIDStr);
    qDebug(qPrintable(mySql));
    myQuery.exec(mySql);
    int selectRet = 0;
    int ele0;
    while(myQuery.next())
    {
        ele0 = myQuery.value(0).toInt();
        qDebug("kira --- Alipay have repeat scan");
        qDebug()<<ele0;//输出id
        selectRet = 1;
        break;
    }

#if 0
    double dsDateTime = tradeTimeStr.toDouble() - MAX_REPEATTIME;
    QString ssDateTime = QString::number((qulonglong)dsDateTime, 10);
#endif

    QDateTime dsDateTime = QDateTime::currentDateTime().addSecs((qint64)-MAX_REPEATTIME);
    QString ssDateTime = dsDateTime.toString("yyyyMMddhhmmss");
    qDebug(qPrintable(ssDateTime));

    mySql= QString("select id from ALIPAYTRADEINFO where TRADETIME>='%1' AND QRCODE='%2';")
                        .arg(ssDateTime).arg(QRcode);
    qDebug(qPrintable(mySql));
    myQuery.exec(mySql);
    while(myQuery.next())
    {
        ele0 = myQuery.value(0).toInt();
        qDebug("kira --- Alipay same QR have repeat scan");
        qDebug()<<ele0; //输出id
        selectRet = 2;
        break;
    }

    myQuery.exec(QObject::tr("drop ALIPAYTRADEINFO"));
    return selectRet;
}

// v2 --- 2017.3.9
int mySqlite::judgeAlipayRepeatScanV2(alipayTradeInfoV2_t tradeInfo, QString QRcode)
{
    int iuserIDLen;
    memcpy(&iuserIDLen, tradeInfo.offlineHeadInfo.userIDLen, 4);
    qDebug("kira --- userIDLen: %d", iuserIDLen);
    QString userIDStr = QByteArray::fromRawData((char*)tradeInfo.userID, iuserIDLen);

    QSqlQuery myQuery(myDb); //以下执行相关SQL语句

#if 0
    QString tradeTimeStr = QByteArray::fromRawData((char*)tradeInfo.offlineHeadInfo.tradeTime, sizeof(tradeInfo.offlineHeadInfo.tradeTime));
    qDebug(qPrintable(tradeTimeStr));
    double dDateTime = tradeTimeStr.toDouble() - MAX_ALIPAY_REPEATTIME;
    QString sDateTime = QString::number((qulonglong)dDateTime, 10);
#endif

    QDateTime DateTime = QDateTime::currentDateTime().addSecs((qint64)-MAX_ALIPAY_REPEATTIME);
    QString sDateTime = DateTime.toString("yyyyMMddhhmmss");
    qDebug(qPrintable(sDateTime));

    QString mySql= QString("select id from ALIPAYTRADEINFOV2 where TRADETIME>='%1' AND USERID='%2';")
                    .arg(sDateTime).arg(userIDStr);
    qDebug(qPrintable(mySql));
    myQuery.exec(mySql);
    int selectRet = 0;
    int ele0;
    while(myQuery.next())
    {
        ele0 = myQuery.value(0).toInt();
        qDebug("kira --- AlipayV2 have repeat scan");
        qDebug()<<ele0;//输出id
        selectRet = 1;
        break;
    }

#if 0
    double dsDateTime = tradeTimeStr.toDouble() - MAX_REPEATTIME;
    QString ssDateTime = QString::number((qulonglong)dsDateTime, 10);
#endif

    QDateTime dsDateTime = QDateTime::currentDateTime().addSecs((qint64)-MAX_REPEATTIME);
    QString ssDateTime = dsDateTime.toString("yyyyMMddhhmmss");
    qDebug(qPrintable(ssDateTime));

    mySql= QString("select id from ALIPAYTRADEINFOV2 where TRADETIME>='%1' AND QRCODE='%2';")
                        .arg(ssDateTime).arg(QRcode);
    qDebug(qPrintable(mySql));
    myQuery.exec(mySql);
    while(myQuery.next())
    {
        ele0 = myQuery.value(0).toInt();
        qDebug("kira --- AlipayV2 same QR have repeat scan");
        qDebug()<<ele0; //输出id
        selectRet = 2;
        break;
    }

    myQuery.exec(QObject::tr("drop ALIPAYTRADEINFOV2"));
    return selectRet;
}

bool mySqlite::updataAlipayServerAck(QString userID, QString tradeTime, QString tradeRecord)
{
    QMutexLocker locker(&sqliteMutex);

    QSqlQuery myQuery(myDb);
    QString mySql = QString("UPDATE ALIPAYTRADEINFO SET SERVERACK = 1 WHERE USERID='%1' AND TRADETIME='%2' AND TRADERECORD='%3';")
                    .arg(userID).arg(tradeTime).arg(tradeRecord);

    if(!myQuery.exec(mySql))
    {
        qDebug()<<"updata Alipay tradeInfo sqlite data failed";
        return false;
    }
    else
        return true;
}

// v2 --- 2017.3.9
bool mySqlite::updataAlipayServerAckV2(QString userID, QString tradeTime, QString tradeRecord)
{
    QMutexLocker locker(&sqliteMutex);

    QSqlQuery myQuery(myDb);
    QString mySql = QString("UPDATE ALIPAYTRADEINFOV2 SET SERVERACK = 1 WHERE USERID='%1' AND TRADETIME='%2' AND TRADERECORD='%3';")
                    .arg(userID).arg(tradeTime).arg(tradeRecord);

    if(!myQuery.exec(mySql))
    {
        qDebug()<<"updata AlipayV2 tradeInfo sqlite data failed";
        return false;
    }
    else
        return true;
}

bool mySqlite::scanAlipayTradeRecordInfo(alipayTradeInfo_t &tradeInfo)
{
    QSqlQuery myQuery(myDb);

    QDateTime DateTime = QDateTime::currentDateTime().addSecs((qint64)-90);
    QString sDateTime = DateTime.toString("yyyyMMddhhmmss");
    QString mySql= QString("select * from ALIPAYTRADEINFO where SERVERACK=0 AND TRADETIME<'%1';").arg(sDateTime);
    qDebug(qPrintable(mySql));
    myQuery.exec(mySql);
    bool selectRet = false;
    int ele0;
    while(myQuery.next())
    {
        ele0 = myQuery.value(0).toInt();
        qDebug("kira --- Alipay have trade record not send to server...");
        qDebug()<<ele0;
        if(tradeInfoIDRecord == ele0)
            continue;
#if 1
        int iuserIDLen = myQuery.value(1).toInt();
        int ioffLineRecordLen = myQuery.value(2).toInt();
        int itradeRecord = myQuery.value(3).toInt();
        memcpy(tradeInfo.offlineHeadInfo.userIDLen, &iuserIDLen, 4);
        memcpy(tradeInfo.offlineHeadInfo.offLineRecordLen, &ioffLineRecordLen, 4);
        memcpy(tradeInfo.offlineHeadInfo.tradeRecord, &itradeRecord, 4);

        //memcpy(tradeInfo.offlineHeadInfo.busline, myQuery.value(4).toByteArray().data(), sizeof(tradeInfo.offlineHeadInfo.busline));
        memcpy(tradeInfo.offlineHeadInfo.busline, myQuery.value(4).toByteArray().data(), myQuery.value(4).toByteArray().length());
        //memcpy(tradeInfo.offlineHeadInfo.busID, myQuery.value(5).toByteArray().data(), sizeof(tradeInfo.offlineHeadInfo.busID));
        memcpy(tradeInfo.offlineHeadInfo.busID, myQuery.value(5).toByteArray().data(), myQuery.value(5).toByteArray().length());
        //memcpy(tradeInfo.offlineHeadInfo.posID, myQuery.value(6).toByteArray().data(), sizeof(tradeInfo.offlineHeadInfo.posID));
        memcpy(tradeInfo.offlineHeadInfo.posID, myQuery.value(6).toByteArray().data(), myQuery.value(6).toByteArray().length());
        //memcpy(tradeInfo.offlineHeadInfo.cityID, myQuery.value(7).toByteArray().data(), sizeof(tradeInfo.offlineHeadInfo.cityID));
        memcpy(tradeInfo.offlineHeadInfo.cityID, myQuery.value(7).toByteArray().data(), myQuery.value(7).toByteArray().length());
        //memcpy(tradeInfo.offlineHeadInfo.tradeTime, myQuery.value(8).toByteArray().data(), sizeof(tradeInfo.offlineHeadInfo.tradeTime));
        memcpy(tradeInfo.offlineHeadInfo.tradeTime, myQuery.value(8).toByteArray().data(), myQuery.value(8).toByteArray().length());

        int iticketPrice = myQuery.value(9).toInt();
        memcpy(tradeInfo.offlineHeadInfo.ticketPrice, &iticketPrice, 4);

        //memcpy(tradeInfo.offlineHeadInfo.longitude, myQuery.value(10).toByteArray().data(), sizeof(tradeInfo.offlineHeadInfo.longitude));
        //memcpy(tradeInfo.offlineHeadInfo.latitude, myQuery.value(11).toByteArray().data(), sizeof(tradeInfo.offlineHeadInfo.latitude));
        memcpy(tradeInfo.offlineHeadInfo.longitude, myQuery.value(10).toByteArray().data(), myQuery.value(10).toByteArray().length());
        memcpy(tradeInfo.offlineHeadInfo.latitude, myQuery.value(11).toByteArray().data(), myQuery.value(11).toByteArray().length());
        memcpy(tradeInfo.offlineHeadInfo.driverID, myQuery.value(12).toByteArray().data(), myQuery.value(12).toByteArray().length());

        memcpy(tradeInfo.userID, myQuery.value(14).toByteArray().data(), myQuery.value(1).toInt());
        memcpy(tradeInfo.record, myQuery.value(15).toByteArray().data(), myQuery.value(2).toInt());
#endif
        selectRet = true;
        break;
    }

    tradeInfoIDRecord = ele0;
    myQuery.exec(QObject::tr("drop ALIPAYTRADEINFO"));
    return selectRet;
}

// v2 --- 2017.3.9
bool mySqlite::scanAlipayTradeRecordInfoV2(alipayTradeInfoV2_t &tradeInfo)
{
    QSqlQuery myQuery(myDb);

    QDateTime DateTime = QDateTime::currentDateTime().addSecs((qint64)-90);
    QString sDateTime = DateTime.toString("yyyyMMddhhmmss");
    QString mySql= QString("select * from ALIPAYTRADEINFOV2 where SERVERACK=0 AND TRADETIME<'%1';").arg(sDateTime);
    qDebug(qPrintable(mySql));
    myQuery.exec(mySql);
    bool selectRet = false;
    int ele0;
    while(myQuery.next())
    {
        ele0 = myQuery.value(0).toInt();
        qDebug("kira --- AlipayV2 have trade record not send to server...");
        qDebug()<<ele0;
        if(tradeInfoIDRecordV2 == ele0)
            continue;
#if 1
        int iuserIDLen = myQuery.value(1).toInt();
        int ioffLineRecordLen = myQuery.value(2).toInt();
        int itradeRecord = myQuery.value(3).toInt();
        memcpy(tradeInfo.offlineHeadInfo.userIDLen, &iuserIDLen, 4);
        memcpy(tradeInfo.offlineHeadInfo.offLineRecordLen, &ioffLineRecordLen, 4);
        memcpy(tradeInfo.offlineHeadInfo.tradeRecord, &itradeRecord, 4);

        int icardNoLen = myQuery.value(4).toInt();
        int icardDataLen = myQuery.value(5).toInt();
        int icardTypeLen = myQuery.value(6).toInt();
        memcpy(tradeInfo.offlineHeadInfo.cardNoLen, &icardNoLen, 4);
        memcpy(tradeInfo.offlineHeadInfo.cardDataLen, &icardDataLen, 4);
        memcpy(tradeInfo.offlineHeadInfo.cardTypeLen, &icardTypeLen, 4);

        memcpy(tradeInfo.offlineHeadInfo.busline, myQuery.value(7).toByteArray().data(), myQuery.value(4).toByteArray().length());
        memcpy(tradeInfo.offlineHeadInfo.busID, myQuery.value(8).toByteArray().data(), myQuery.value(5).toByteArray().length());
        memcpy(tradeInfo.offlineHeadInfo.posID, myQuery.value(9).toByteArray().data(), myQuery.value(6).toByteArray().length());
        memcpy(tradeInfo.offlineHeadInfo.cityID, myQuery.value(10).toByteArray().data(), myQuery.value(7).toByteArray().length());
        memcpy(tradeInfo.offlineHeadInfo.tradeTime, myQuery.value(11).toByteArray().data(), myQuery.value(8).toByteArray().length());

        int iticketPrice = myQuery.value(12).toInt();
        memcpy(tradeInfo.offlineHeadInfo.ticketPrice, &iticketPrice, 4);

        memcpy(tradeInfo.offlineHeadInfo.longitude, myQuery.value(13).toByteArray().data(), myQuery.value(10).toByteArray().length());
        memcpy(tradeInfo.offlineHeadInfo.latitude, myQuery.value(14).toByteArray().data(), myQuery.value(11).toByteArray().length());
        memcpy(tradeInfo.offlineHeadInfo.driverID, myQuery.value(15).toByteArray().data(), myQuery.value(12).toByteArray().length());

        memcpy(tradeInfo.userID, myQuery.value(16).toByteArray().data(), myQuery.value(1).toInt());
        memcpy(tradeInfo.record, myQuery.value(17).toByteArray().data(), myQuery.value(2).toInt());

        memcpy(tradeInfo.cardNo, myQuery.value(18).toByteArray().data(), myQuery.value(4).toInt());
        memcpy(tradeInfo.cardData, myQuery.value(19).toByteArray().data(), myQuery.value(5).toInt());
        memcpy(tradeInfo.cardType, myQuery.value(20).toByteArray().data(), myQuery.value(6).toInt());
#endif
        selectRet = true;
        break;
    }

    tradeInfoIDRecordV2 = ele0;
    myQuery.exec(QObject::tr("drop ALIPAYTRADEINFOV2"));
    return selectRet;
}

bool mySqlite::insertAlipayAbnormalQR(QString QRstr, QString datetimeStr, int keyret, int cryptret)
{
    QMutexLocker locker(&sqliteMutex);

    QSqlQuery myQuery(myDb);
    myQuery.prepare("INSERT INTO ALIPAYABNORMALINFO (SERVERACK, KEYGETRET, DECRYPTRET, DATETIME, QRCODE)"
                    "VALUES (:SERVERACK, :KEYGETRET, :DECRYPTRET, :DATETIME, :QRCODE);");

    myQuery.bindValue(":SERVERACK", 0);
    myQuery.bindValue(":KEYGETRET", keyret);
    myQuery.bindValue(":DECRYPTRET", cryptret);
    myQuery.bindValue(":DATETIME", datetimeStr);
    myQuery.bindValue(":QRCODE", QRstr);

    if(!myQuery.exec())
    {
        qDebug()<<"insert Alipay abnormalinfo sqlite data failed";
        return false;
    }
    else
        return true;
}

bool mySqlite::clearAlipayOutAbnormalQR()
{ 
    QDateTime DateTime = QDateTime::currentDateTime().addDays((qint64)-abnormalOuttime.toDouble());
    QString sDateTime = DateTime.toString("yyyyMMddhhmmss");

    QSqlQuery myQuery(myDb); //以下执行相关SQL语句
    QString mySql= QString("delete from ALIPAYABNORMALINFO where DATETIME<='%1';").arg(sDateTime);
    qDebug(qPrintable(mySql));
    if(!myQuery.exec(mySql))
    {
        qDebug()<<" alipay abnormalQR outtime sqlite data failed";
        return false;
    }
    else
        return true;
}

bool mySqlite::clearAlipayOutTradeInfo()
{
    QDateTime DateTime = QDateTime::currentDateTime().addDays((qint64)-tradeOuttime.toDouble());
    QString sDateTime = DateTime.toString("yyyyMMddhhmmss");
    QSqlQuery myQuery(myDb); //以下执行相关SQL语句
    QString mySql= QString("delete from ALIPAYTRADEINFO where TRADETIME<='%1' AND SERVERACK=1;").arg(sDateTime);
    qDebug(qPrintable(mySql));
    if(!myQuery.exec(mySql))
    {
        qDebug()<<"clear alipay trade outtime sqlite data failed";
        return false;
    }
    else
        return true;
}

// V2 --- 2017.3.9
bool mySqlite::clearAlipayOutTradeInfoV2()
{
    QDateTime DateTime = QDateTime::currentDateTime().addDays((qint64)-tradeOuttime.toDouble());
    QString sDateTime = DateTime.toString("yyyyMMddhhmmss");
    QSqlQuery myQuery(myDb); //以下执行相关SQL语句
    QString mySql= QString("delete from ALIPAYTRADEINFOV2 where TRADETIME<='%1' AND SERVERACK=1;").arg(sDateTime);
    qDebug(qPrintable(mySql));
    if(!myQuery.exec(mySql))
    {
        qDebug()<<"clear alipayV2 trade outtime sqlite data failed";
        return false;
    }
    else
        return true;
}

bool mySqlite::processDriverSign(driveSignNetInfo_t &signNetInfo)
{
    QSqlQuery myQuery(myDb); //以下执行相关SQL语句
    QDateTime DateTime = QDateTime::currentDateTime().addDays((qint64)-1);
    QString sDateTime = DateTime.toString("yyyyMMddhhmmss");
    qDebug(qPrintable(sDateTime));

    QString mySql= QString("select DRIVERID, SIGNFLAG, DRIVERTYPE from DRIVESIGNINFO where SIGNTIME>='%1';")
                    .arg(sDateTime);
//    QString mySql= QString("select DRIVERID, SIGNFLAG from DRIVESIGNINFO where SIGNTIME>='%1';")
//                    .arg(sDateTime);
    qDebug(qPrintable(mySql));
    myQuery.exec(mySql);
    QString ldriverID;
    int lsignflag;
    bool ret;
    bool selectRet = false;
    int iCardSign = 0;
    while(myQuery.next())
    {
        ldriverID = myQuery.value(0).toString();
        lsignflag = myQuery.value(1).toInt();
        iCardSign = myQuery.value(2).toInt();
        selectRet = true;
    }
    myQuery.exec(QObject::tr("drop DRIVESIGNINFO"));
    if(selectRet)
    {
        QString driveIDIDStr = QByteArray::fromRawData((char*)signNetInfo.driveID, sizeof(signNetInfo.driveID));
        if(iCardSign != 0)//prev is card sign
        {
            if((ldriverID == driveIDIDStr) || (unsigned char)signNetInfo.driverType==0x12)
            {
                if('0' == lsignflag)
                {
                    signNetInfo.signFlag = '1';
                }
                else
                {
                    signNetInfo.signFlag = '0';
                }
                ret = insertDriverSignInfo(signNetInfo);
                if(false == ret)
                    return false;
                return true;
            }
            printf("qwy --- driver sign is fail (driverid is diff and is not admin)");
            return false;
        }
        if(ldriverID == driveIDIDStr)
        {
            qDebug("kira --- lsignflag: %d", lsignflag);
            if('0' == lsignflag)
            {
                signNetInfo.signFlag = '1';
            }
            else
            {
                signNetInfo.signFlag = '0';
            }
            ret = insertDriverSignInfo(signNetInfo);
            if(false == ret)
                return false;
        }
        else
        {
            if('1' == lsignflag)
            {
                driveSignNetInfo_t lsignNetInfo;
                memcpy(&lsignNetInfo, &signNetInfo, sizeof(driveSignNetInfo_t));
                memcpy(&lsignNetInfo.driveID, ldriverID.toLatin1().data(), ldriverID.length());
                lsignNetInfo.signFlag = '0';
                ret = insertDriverSignInfo(lsignNetInfo);
                if(false == ret)
                    return false;
            }

            signNetInfo.signFlag = '1';
            ret = insertDriverSignInfo(signNetInfo);
            if(false == ret)
                return false;
        }
        return true;
    }
    else
    {
        qDebug()<<"no driver sign record sqlite data...";
        signNetInfo.signFlag = '1';
        ret = insertDriverSignInfo(signNetInfo);
        return ret;
    }
}

bool mySqlite::insertDriverSignInfo(driveSignNetInfo_t signNetInfo)
{
    QMutexLocker locker(&sqliteMutex);

    QString posIDStr = QByteArray::fromRawData((char*)signNetInfo.posID, sizeof(signNetInfo.posID));
    QString driveIDStr = QByteArray::fromRawData((char*)signNetInfo.driveID, sizeof(signNetInfo.driveID));
    QString signTimeStr = QByteArray::fromRawData((char*)signNetInfo.signTime, sizeof(signNetInfo.signTime));
    QString buslineStr = QByteArray::fromRawData((char*)signNetInfo.busline, sizeof(signNetInfo.busline));
    QString busIDStr = QByteArray::fromRawData((char*)signNetInfo.busID, sizeof(signNetInfo.busID));
    QString cityIDStr = QByteArray::fromRawData((char*)signNetInfo.cityID, sizeof(signNetInfo.cityID));
    QString longitudeStr = QByteArray::fromRawData((char*)signNetInfo.longitude, sizeof(signNetInfo.longitude));
    QString latitudeStr = QByteArray::fromRawData((char*)signNetInfo.latitude, sizeof(signNetInfo.latitude));

    QSqlQuery myQuery(myDb);
//    myQuery.prepare("INSERT INTO DRIVESIGNINFO (POSID,DRIVERID,SIGNFLAG,SIGNTIME,BUSLINE,BUSID,CITYID,LONGITUDE,LATITUDE,SERVERACK)"
//                    "VALUES (:POSID, :DRIVERID, :SIGNFLAG, :SIGNTIME, :BUSLINE, :BUSID, :CITYID, :LONGITUDE, :LATITUDE, :SERVERACK);");

    myQuery.prepare("INSERT INTO DRIVESIGNINFO (POSID,DRIVERID,SIGNFLAG,SIGNTIME,BUSLINE,BUSID,CITYID,LONGITUDE,LATITUDE,SERVERACK,DRIVERTYPE)"
                    "VALUES (:POSID, :DRIVERID, :SIGNFLAG, :SIGNTIME, :BUSLINE, :BUSID, :CITYID, :LONGITUDE, :LATITUDE, :SERVERACK, :DRIVERTYPE);");
#if 1
    myQuery.bindValue(":POSID", posIDStr);
    myQuery.bindValue(":DRIVERID", driveIDStr);
    myQuery.bindValue(":SIGNFLAG", signNetInfo.signFlag);
    myQuery.bindValue(":SIGNTIME", signTimeStr);
    myQuery.bindValue(":BUSLINE", buslineStr);
    myQuery.bindValue(":BUSID", busIDStr);
    myQuery.bindValue(":CITYID", cityIDStr);
    myQuery.bindValue(":LONGITUDE", longitudeStr);
    myQuery.bindValue(":LATITUDE", latitudeStr);
    myQuery.bindValue(":SERVERACK", 0);
    myQuery.bindValue(":DRIVERTYPE",signNetInfo.driverType);
#endif
    if(!myQuery.exec())
    {
        qDebug()<<"insert drive sign record sqlite data failed";
        return false;
    }
    else
        return true;
}

bool mySqlite::updataDriverSignAck(QString driverID, QString signTime)
{
    QMutexLocker locker(&sqliteMutex);

    QSqlQuery myQuery(myDb);
    QString mySql = QString("UPDATE DRIVESIGNINFO SET SERVERACK = 1 WHERE DRIVERID='%1' AND SIGNTIME='%2';")
                    .arg(driverID).arg(signTime);

    if(!myQuery.exec(mySql))
    {
        qDebug()<<"updata driver sign record sqlite data failed";
        return false;
    }
    else
        return true;
}

bool mySqlite::scanDriverSignInfo(driveSignNetInfo_t &signNetInfo)
{
    QSqlQuery myQuery(myDb);

    QDateTime DateTime = QDateTime::currentDateTime().addSecs((qint64)-90);
    QString sDateTime = DateTime.toString("yyyyMMddhhmmss");
    QString mySql= QString("select * from DRIVESIGNINFO where SERVERACK=0 AND SIGNTIME<'%1';").arg(sDateTime);
    qDebug(qPrintable(mySql));
    myQuery.exec(mySql);
    bool selectRet = false;
    int ele0;
    while(myQuery.next())
    {
        ele0 = myQuery.value(0).toInt();
        qDebug("kira --- Alipay have trade record not send to server...");
        qDebug()<<ele0;
        if(driverSignIDRecord == ele0)
            continue;
#if 1
        memcpy(signNetInfo.posID, myQuery.value(1).toByteArray().data(), myQuery.value(1).toByteArray().length());
        memcpy(signNetInfo.driveID, myQuery.value(2).toByteArray().data(), myQuery.value(2).toByteArray().length());
        signNetInfo.signFlag = (char)myQuery.value(3).toInt();
        memcpy(signNetInfo.signTime, myQuery.value(4).toByteArray().data(), myQuery.value(4).toByteArray().length());
        memcpy(signNetInfo.busline, myQuery.value(5).toByteArray().data(), myQuery.value(5).toByteArray().length());
        memcpy(signNetInfo.busID, myQuery.value(6).toByteArray().data(), myQuery.value(6).toByteArray().length());
        memcpy(signNetInfo.cityID, myQuery.value(7).toByteArray().data(), myQuery.value(7).toByteArray().length());
        memcpy(signNetInfo.longitude, myQuery.value(8).toByteArray().data(), myQuery.value(8).toByteArray().length());
        memcpy(signNetInfo.latitude, myQuery.value(9).toByteArray().data(), myQuery.value(9).toByteArray().length());
        signNetInfo.driverType = (char)myQuery.value(10).toByteArray().toInt();
#endif
        selectRet = true;
        break;
    }

    driverSignIDRecord = ele0;
    myQuery.exec(QObject::tr("drop DRIVESIGNINFO"));
    return selectRet;
}

bool mySqlite::initDriverSignInfo(QString &driverID)
{
    QSqlQuery myQuery(myDb); //以下执行相关SQL语句
    QDateTime DateTime = QDateTime::currentDateTime().addDays((qint64)-1);
    QString sDateTime = DateTime.toString("yyyyMMddhhmmss");
    qDebug(qPrintable(sDateTime));

    //QString mySql= QString("select DRIVERID from DRIVESIGNINFO where SIGNTIME>='%1' AND SIGNFLAG='%2';")
    //                .arg(sDateTime).arg(49);
    QString mySql= QString("select DRIVERID, SIGNFLAG from DRIVESIGNINFO where SIGNTIME>='%1';")
                    .arg(sDateTime);
    qDebug(qPrintable(mySql));
    myQuery.exec(mySql);
    QString ldriverID;
    bool selectRet = false;
    int signFlag = 0;

    while(myQuery.next())
    {
        //ldriverID = myQuery.value(0).toString();
        ldriverID = myQuery.value(0).toByteArray().data();
        selectRet = true;
        signFlag = myQuery.value(1).toInt();
    }
    if((selectRet) && (signFlag == 49))
        driverID = ldriverID;
    else
        driverID = " ";

    qDebug("kira --- initial driverID: %d", signFlag);
    qDebug(qPrintable(driverID));

    myQuery.exec(QObject::tr("drop DRIVESIGNINFO"));
    return selectRet;
}

bool mySqlite::judgeDriverSignRepeat(QString driverID)
{
    QSqlQuery myQuery(myDb);//以下执行相关SQL语句
    QDateTime DateTime = QDateTime::currentDateTime().addSecs((qint64)-MAX_DRIVER_REPEATTIME);
    QString dateTime = DateTime.toString("yyyyMMddhhmmss");

    QString mySql = QString("select id from DRIVESIGNINFO where SIGNTIME>='%1' AND DRIVERID='%2';")
                        .arg(dateTime).arg(driverID);
    qDebug(qPrintable(mySql));
    myQuery.exec(mySql);
    bool selectRet = false;
    while(myQuery.next())//query.next()指向查找到的第一条记录，然后每次后移一条记录
    {
        int ele0 = myQuery.value(0).toInt();    //query.value(0)是id的值，将其转换为int型
        qDebug()<<ele0;
        selectRet = true;
    }
    myQuery.exec(QObject::tr("drop DRIVESIGNINFO"));
    return selectRet;
}

bool mySqlite::clearDriverSignInfo()
{
    QDateTime DateTime = QDateTime::currentDateTime().addDays((qint64)-driverSignOuttime.toDouble());
    QString sDateTime = DateTime.toString("yyyyMMddhhmmss");

    QSqlQuery myQuery(myDb); //以下执行相关SQL语句
    QString mySql= QString("delete from DRIVESIGNINFO where SIGNTIME<='%1' AND SERVERACK=1;").arg(sDateTime);
    qDebug(qPrintable(mySql));
    if(!myQuery.exec(mySql))
    {
        qDebug()<<"clear driver sign record outtime sqlite data failed";
        return false;
    }
    else
        return true;
}

//銀聯信息表相關函數
bool mySqlite::insertUnionpayInfo(unionPayInfo_t& tag)
{
    QString busline = QByteArray::fromRawData(tag.busline,sizeof(tag.busline));
    QString busid = QByteArray::fromRawData(tag.busID,sizeof(tag.busID));
    QString posid = QByteArray::fromRawData(tag.posID,sizeof(tag.posID));
    QString cityid = QByteArray::fromRawData(tag.cityID,sizeof(tag.cityID));
    QString driverid = QByteArray::fromRawData(tag.driverID,sizeof(tag.driverID));
    QString tradetime = QByteArray::fromRawData(tag.tradeTime,sizeof(tag.tradeTime));
    QByteArray ticketprice = QByteArray::fromRawData((const char*)tag.ticketPrice,sizeof(tag.ticketPrice));
    QString longitudeStr = QByteArray::fromRawData(tag.longitude, sizeof(tag.longitude));
    QString latitudeStr = QByteArray::fromRawData(tag.latitude, sizeof(tag.latitude));
    QString cardno = QByteArray::fromRawData(tag.cardNo,sizeof(tag.cardNo));
    QByteArray serialno = QByteArray::fromRawData((const char*)tag.serialNo,sizeof(tag.serialNo));
    QByteArray validdate = QByteArray::fromRawData((const char*)tag.validDate,sizeof(tag.validDate));
    QString settlementDate = QByteArray::fromRawData(tag.settlementDate,sizeof(tag.settlementDate));
    QString cardCenterTime = QByteArray::fromRawData(tag.cardCenterTime,sizeof(tag.cardCenterTime));
    QString terminalId = QByteArray::fromRawData(tag.terminalId,sizeof(tag.terminalId));
    QString merchantId = QByteArray::fromRawData(tag.merchantId,sizeof(tag.merchantId));
    QString currencyCode = QByteArray::fromRawData(tag.currencyCode,sizeof(tag.currencyCode));
    QString cardCenterSerialNo = QByteArray::fromRawData(tag.cardCenterSerialNo,sizeof(tag.cardCenterSerialNo));
    QString retCode = QByteArray::fromRawData(tag.retCode,sizeof(tag.retCode));
    QByteArray amount = QByteArray::fromRawData((const char*)tag.tradeAmount,sizeof(tag.tradeAmount));

    QSqlQuery myQuery(myDb);
    myQuery.prepare("INSERT INTO UNIONPAYINFO (SERVERACK,BUSLINE,BUSID,POSID,CITYID,DRIVERID,TRADETIME,TICKETPRICE,LONGITUDE,LATITUDE,CARDNO,SERIALNO,VALIDDATE,TRADETYPE,TRADENATURE,SETTLEMENTDATE,CARDCENTERTIME,TERMINALID,MERCHANTID,CURRENCYCODE,CARDCENTERSERIALNO,RETCODE,AMOUNT)"
                    "VALUES (:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,:15,:16,:17,:18,:19,:20,:21,:22,:23);");
    myQuery.bindValue(":1", 0);
    myQuery.bindValue(":2", busline);
    myQuery.bindValue(":3", busid);
    myQuery.bindValue(":4", posid);
    myQuery.bindValue(":5", cityid);
    myQuery.bindValue(":6", driverid);
    myQuery.bindValue(":7", tradetime);
    myQuery.bindValue(":8", ticketprice);
    myQuery.bindValue(":9", longitudeStr);
    myQuery.bindValue(":10", latitudeStr);
    myQuery.bindValue(":11",cardno);
    myQuery.bindValue(":12",serialno);
    myQuery.bindValue(":13",validdate);
    myQuery.bindValue(":14",tag.tradeType);
    myQuery.bindValue(":15",tag.tradeNature);
    myQuery.bindValue(":16",settlementDate);
    myQuery.bindValue(":17",cardCenterTime);
    myQuery.bindValue(":18",terminalId);
    myQuery.bindValue(":19",merchantId);
    myQuery.bindValue(":20",currencyCode);
    myQuery.bindValue(":21",cardCenterSerialNo);
    myQuery.bindValue(":22",retCode);
    myQuery.bindValue(":23",amount);
    if(!myQuery.exec())
    {
        qDebug()<<"insert unionpay information sqlite data failed";
        return false;
    }
    return true;
}

bool mySqlite::selectUnionpayInfo(unionPayInfo_t& tag)
{
    QSqlQuery myQuery(myDb);//以下执行相关SQL语句
    QString mySql = QString("select * from UNIONPAYINFO where SERVERACK=0;");
    qDebug(qPrintable(mySql));
    myQuery.exec(mySql);
    bool selectRet = false;
    while(myQuery.next())//query.next()指向查找到的第一条记录，然后每次后移一条记录
    {
        int ele0 = myQuery.value(0).toInt();    //query.value(0)是id的值，将其转换为int型
        qDebug()<<ele0;
        memcpy(tag.busline,myQuery.value(2).toByteArray().data(),myQuery.value(2).toByteArray().length());
        memcpy(tag.busID,myQuery.value(3).toByteArray().data(),myQuery.value(3).toByteArray().length());
        memcpy(tag.posID,myQuery.value(4).toByteArray().data(),myQuery.value(4).toByteArray().length());
        memcpy(tag.cityID,myQuery.value(5).toByteArray().data(),myQuery.value(5).toByteArray().length());
        memcpy(tag.driverID,myQuery.value(6).toByteArray().data(),myQuery.value(6).toByteArray().length());
        memcpy(tag.tradeTime,myQuery.value(7).toByteArray().data(),myQuery.value(7).toByteArray().length());
        memcpy(tag.ticketPrice,myQuery.value(8).toByteArray().data(),myQuery.value(8).toByteArray().length());
        memcpy(tag.longitude,myQuery.value(9).toByteArray().data(),myQuery.value(9).toByteArray().length());
        memcpy(tag.latitude,myQuery.value(10).toByteArray().data(),myQuery.value(10).toByteArray().length());
        memcpy(tag.cardNo,myQuery.value(11).toByteArray().data(),myQuery.value(11).toByteArray().length());
        memcpy(tag.serialNo,myQuery.value(12).toByteArray().data(),myQuery.value(12).toByteArray().length());
        memcpy(tag.validDate,myQuery.value(13).toByteArray().data(),myQuery.value(13).toByteArray().length());
        tag.tradeType = (char)myQuery.value(14).toInt();
        tag.tradeNature = (char)myQuery.value(15).toInt();
        memcpy(tag.settlementDate,myQuery.value(16).toByteArray().data(),myQuery.value(16).toByteArray().length());
        memcpy(tag.cardCenterTime,myQuery.value(17).toByteArray().data(),myQuery.value(17).toByteArray().length());
        memcpy(tag.terminalId,myQuery.value(18).toByteArray().data(),myQuery.value(18).toByteArray().length());
        memcpy(tag.merchantId,myQuery.value(19).toByteArray().data(),myQuery.value(19).toByteArray().length());
        memcpy(tag.currencyCode,myQuery.value(20).toByteArray().data(),myQuery.value(20).toByteArray().length());
        memcpy(tag.cardCenterSerialNo,myQuery.value(21).toByteArray().data(),myQuery.value(21).toByteArray().length());
        memcpy(tag.retCode,myQuery.value(22).toByteArray().data(),myQuery.value(22).toByteArray().length());
        memcpy(tag.tradeAmount,myQuery.value(23).toByteArray().data(),myQuery.value(23).toByteArray().length());
        selectRet = true;
        break;
    }
    myQuery.exec(QObject::tr("drop UNIONPAYINFO"));
    return selectRet;
}

bool mySqlite::updateUnionpayInfoServerAck(QString cardno,QString tradetime)
{
    QMutexLocker locker(&sqliteMutex);

    QSqlQuery myQuery(myDb);
    QString mySql = QString("UPDATE UNIONPAYINFO SET SERVERACK = 1 WHERE CARDNO='%1' AND TRADETIME='%2';")
                    .arg(cardno).arg(tradetime);

    if(!myQuery.exec(mySql))
    {
        qDebug()<<"updata UNIONPAYINFO record sqlite data failed";
        return false;
    }
    return true;
}

bool mySqlite::clearUnionpayInfo()
{
    QSqlQuery myQuery(myDb);
    QString mySql = QString("DELETE FROM UNIONPAYINFO WHERE SERVERACK=1");

    if(!myQuery.exec(mySql))
    {
        qDebug() << "clear unionpayinfo failed";
        return false;
    }
    return true;
}
