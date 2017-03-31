#ifndef MYPOSWIDGET_H
#define MYPOSWIDGET_H

#include <QWidget>
#include <QMap>
#include <QLibrary>
#include "poswidget.h"
#include "tradedialog.h"
#include "scanserialtalk.h"
#include "setconfig.h"
#include "stmserialtalk.h"
#include "gprsserialtalk.h"
#include "gpsserialtalk.h"
#include "mysqlite.h"
#include "datastructure.h"
#include "signalwidget.h"
#include "blackclearwidget.h"
#include "busidwidget.h"
#include "buslinewidget.h"
#include "posidwidget.h"
#include "unionpay.h"
#include "unionpaystructure.h"
#include "networktalk.h"
#include "appUpdater.h"

class QTimer;
namespace Ui {
class myPosWidget;
}

#if 0
typedef struct posFtpInfo_s
{
    QString version;
    QString ftpIp;
    QString ftpPort;
    QString ftpPath;
    QString ftpUser;
    QString ftpPassword;
    QString crc;
}posFtpInfo_t;
#endif

class myPosWidget : public QWidget
{
    Q_OBJECT

public:
    explicit myPosWidget(QWidget *parent = 0);
    ~myPosWidget();
    unsigned char AscToHex(unsigned char aChar);
    unsigned char HexToAsc(unsigned char aHex);
    int processTrade();
    int processAlipayScanInfo();
    int processAlipayScanInfoV2();  // kira --- 2017.3.9
    bool insertTransactionData(scanTradeInfo tradeInfo, consumeInfo_t consumeInfo);
    void get_key_from_key_id(int key_id, char* master_pub_key);
    int initAlipayKey();
    int initAlipayBlack();
    int initAlipayCardType();   // v2 --- 2017.3.10
    int Hex_String_to_Bytes(
        char* hex_string,
        int hex_string_len,
        unsigned char* bytes,
        int bytes_len);
    unsigned char Hex_of_Char(char c);
    void mock_qrcode(unsigned char* qrcode, int* qrcode_len);
    void askAlipayKeylist();
    void askAlipayBlacklist();
    void askAlipayCardTypelist();   // v2 --- 2017.3.10
    void dealDriveSign();
    //  pos param
    void ackPosParamSet();
    // version updata
    void askPosLVersionUpdata();
    void askPosSVersionUpdata();

public slots:
    void showTradeDialog();
    void showPosWidget();
    void processBeatInfo();
    void processBeatAck_v1();
    void processConsumeAck_v1();

    // 微信在线支付
    void processWeixinScanInfo();
    void processWeixinBeatAck_v1();
    void processWeixinTradeAck_v1();

    // Alipay
    void processAlipayTradeAck_v1();
    void processAlipayKeyInfo_v1();
    void processAlipayBlackInfo_v1();
    void processAlipayBlackInfo_v2();
    void processAlipayCardType_v1();    // v2 --- 2017.3.10
    //unionpay
    void processUnionpayAck_v1(unionPayAck_t tag);

    // ui
    void processLongpress();
    void processShortpress();
    void processLeftpress();
    void processRightpress();
    void processCheckConfirmBusID();
    void processCheckConfirmPosID();
    void processCheckConfirmBusline();

    // drive sign
    void processDriveSign();
    void processDriveSign_v2(QString strDriver, unsigned char cardtype);
    void processDriverSignAck_v1();

    // pos param
    void processPosParamSet();
    void processPosParamSet_v2(posParam_t posParam);

    // ftp info
    void processPosFtpInfo();

    // up
    void processUpdataInfoAck();

    // pscam
    void processPscamAck(char retAck);

    //update stm complete
    void processStmUpdateSuccess();

public slots:
    void onTimerOut();
    void failTimerOut();
    void tradeScanTimerOut();
    void tradeScanTimerOutV2();
    void signScanTimerOut();
    void onlineValidTimerOut();

private:
    Ui::myPosWidget *ui;

public:
    //const QString configFile = "./configure/setting.ini";
    //const QString servergroup = "server";
    //const QString workgroup = "work";
    QString configFile;
    QString servergroup;
    QString workgroup;
    //Alipay
    QString Aliapygroup;
    //DB
    QString dbgroup;

public:
    void InitialConfig();
    QString busID;
    QString buslineID;
    QString posID;
    QString tickets;
    QString monthrate;
    QString walletrate;
    QString serverIP;
    QString serverPort;
    QString beatInterval;
    QString keyBatch;
    QString cityID;
    QString driverID;
    unsigned char driverCardType;//0x11:司机A卡 0x12:管理员卡 0x13司机B卡 0x91:调度总卡
    QString LVersion;   // kira added - 2017.3.7
    QString SVersion;
    QString LTempVersion;   // kira added - 2017.3.14
    QString STempVersion;
    //Alipay
    QString blacklistVer;
    QString keylistVer;
    QString blackCurrentNum;
    QString keyCurrentNum;
    //DB
    QString abnormalOuttime;
    QString tradeOuttime;
    QString driverSignOuttime;

public:
    QTimer *myTimer;
    QTimer *failTimer;
    QTimer *tradeScanTimer;
    QTimer *signScanTimer;
    QTimer *onlineValidTimer;
    scanSerialTalk* scanPosScanTalk;
    stmSerialTalk* scanPosStmTalk;
    gprsSerialTalk* scanPosGprsTalk;
    gpsSerialTalk* scanPosGpsTalk;
    tradeDialog* scanPosTradeTalk;
    posWidget* scanPosTalk;
    mySqlite* scanPosDB;
    NetworkTalk* scanPosNetTalk;
    setConfig configSet;
    QString balance;
    QString debit;
    bool isShowPosWidget;
    QMap<int, QString> mykeyMap;
    QMap<int, QString> myblackMap;
    QMap<int, QString> mycardTypeMap;       // v2 --- 2017.3.10
    QMap<int, QString> onlineScanInfoMap;
    int onlineScanInfoNum;
    int tradeRecord;
    int onlineTradeRecord;
    QLibrary *posoffline_lib;
    void* dp;
    bool isAskKey;
    bool isAskBlack;
    uiState myui;
    bool isClearBlankBlack;
    bool isOnlineTradeIdle;

    signalWidget* scanPosSignal;
    blackClearWidget* scanPosBlackClear;
    busidWidget* scanPosBusID;
    buslineWidget* scanPosBusline;
    posidWidget* scanPosPosID;

public:
    cardreceiveA2 mycardreceiveA2;
    unionPay* scanPosUnionPay;

public slots:
    void processUnionPayTradeInfo();
    void processUnionPayTermMkInfo();

public:
    appUpdater myAppUpdater;
    bool judgeUp;
    bool judgeUpStm;
    bool judgeFtpWork;
    int sendUpdataInfo(int type);

public:
    alipayTradeInfoV2_t vCityCardInfo;
    QString cityCardQR;
    //當前是否處於簽到狀態
    bool m_bDriverSigned;
};

#endif // MYPOSWIDGET_H
