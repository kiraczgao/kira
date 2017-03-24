#include "myposwidget.h"
#include "ui_myposwidget.h"
#include <QTimer>
#include <time.h>
#include <QDateTime>
#include <QStringList>
#include <QList>
#include <QStringListIterator>
#include <sys/time.h>
#include <dlfcn.h>
#include <pos_crypto.h>
#include "timertesting.h"

#define ALIPAYBLACK_DEBUG

myPosWidget::myPosWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::myPosWidget)
{
    ui->setupUi(this);
    //自定义数据结构
    qRegisterMetaType<unionPayAck_t> ("unionPayAck_t");

    isShowPosWidget = true;
    // 初始化参数
    this->InitialConfig();
    qDebug("kira --- LInux version: ");
    qDebug(qPrintable(LVersion));
    qDebug("kira --- STM32 version: ");
    qDebug(qPrintable(SVersion));

    qDebug("kira --- read tickets: ");
    qDebug(qPrintable(tickets));
    // 初始化待机UI
    scanPosTalk = new posWidget();
    scanPosTalk->setTickets(tickets);
    scanPosTalk->setBusID(busID);
    QString version = LVersion;
    version += " ";
    version += SVersion;
    scanPosTalk->setVersion(version);
    // 初始化交易UI
    scanPosTradeTalk = new tradeDialog();
    connect(scanPosTradeTalk, SIGNAL(backPosWidget()), this, SLOT(showPosWidget()));
    // 初始化扫码枪串口
    scanPosScanTalk = new scanSerialTalk;
    connect(scanPosScanTalk, SIGNAL(recvScanInfo()), this, SLOT(showTradeDialog()));
    connect(scanPosScanTalk, SIGNAL(recvDriveSignInfo()), this, SLOT(processDriveSign()));
    scanPosScanTalk->start();
    // 微信在线支付
    connect(scanPosScanTalk, SIGNAL(recvWeixinScanInfo()), this, SLOT(processWeixinScanInfo()));
    // Alipay
    //connect(scanPosScanTalk, SIGNAL(recvAlipayScanInfo()), this, SLOT(processAlipayScanInfo()));
    // 初始化stm串口
    scanPosStmTalk = new stmSerialTalk;
    connect(scanPosStmTalk, SIGNAL(recvCardInfo()), this, SLOT(showTradeDialog()));
    connect(scanPosStmTalk, SIGNAL(recvLongpress()), this, SLOT(processLongpress()));
    connect(scanPosStmTalk, SIGNAL(recvShortpress()), this, SLOT(processShortpress()));
    connect(scanPosStmTalk, SIGNAL(recvLeftpress()), this, SLOT(processLeftpress()));
    connect(scanPosStmTalk, SIGNAL(recvRightpress()), this, SLOT(processRightpress()));
    connect(scanPosStmTalk, SIGNAL(recvCardTradeInfoA2()), this, SLOT(processUnionPayTradeInfo()));
    connect(scanPosStmTalk, SIGNAL(recvTermMkInfoA3()), this, SLOT(processUnionPayTermMkInfo()));
    // pscam
    connect(scanPosStmTalk, SIGNAL(recvPscamAck(char)), this, SLOT(processPscamAck(char)));
    scanPosStmTalk->setTickets(tickets);
    scanPosStmTalk->start();

#if 1
    // 初始化 UnionPay
    scanPosUnionPay = new unionPay;
    connect(scanPosUnionPay, SIGNAL(unionPayTradeFailed()), scanPosStmTalk, SLOT(unionPayTradeFailed()));
    connect(scanPosUnionPay, SIGNAL(unionPayTradeSuccess()), scanPosStmTalk, SLOT(unionPayTradeSuccess()));
    connect(scanPosUnionPay, SIGNAL(unionPayA1Talk()), scanPosStmTalk, SLOT(processUnionPayProcA1()));
    scanPosUnionPay->initUnionPayConfig();
    scanPosUnionPay->start();
#endif

#if 0
    // 初始化GPRS串口
    scanPosGprsTalk = new gprsSerialTalk;
    scanPosGprsTalk->setServerIP(serverIP);
    scanPosGprsTalk->setServerPort(serverPort);
    scanPosGprsTalk->InitialGprs();
    connect(scanPosGprsTalk, SIGNAL(sendBeatInfo()), this, SLOT(processBeatInfo()));
    connect(scanPosGprsTalk, SIGNAL(recvBeatAck_v1()), this, SLOT(processBeatAck_v1()));
    connect(scanPosGprsTalk, SIGNAL(recvConsumeAck_v1()), this, SLOT(processConsumeAck_v1()));
    scanPosGprsTalk->start();

    // 微信在线支付
    connect(scanPosGprsTalk, SIGNAL(recvWeixinBeatAck_v1()), this, SLOT(processWeixinBeatAck_v1()));
    connect(scanPosGprsTalk, SIGNAL(recvWeixinTradeAck_v1()), this, SLOT(processWeixinTradeAck_v1()));
    // Alipay
    connect(scanPosGprsTalk, SIGNAL(recvAlipayTradeAck_v1()), this, SLOT(processAlipayTradeAck_v1()));
    connect(scanPosGprsTalk, SIGNAL(recvAlipayKeyInfo_v1()), this, SLOT(processAlipayKeyInfo_v1()));
    connect(scanPosGprsTalk, SIGNAL(recvAlipayBlackInfo_v1()), this, SLOT(processAlipayBlackInfo_v2()));
    // driver sign
    connect(scanPosGprsTalk, SIGNAL(recvDriverSignInfo_v1()), this, SLOT(processDriverSignAck_v1()));
 #endif

#if 1
    scanPosNetTalk = new NetworkTalk;
    scanPosNetTalk->setServerIP(serverIP);
    scanPosNetTalk->setServerPort(serverPort);
    connect(scanPosNetTalk, SIGNAL(sendBeatInfo()), this, SLOT(processBeatInfo()));
    connect(scanPosNetTalk, SIGNAL(recvBeatAck_v1()), this, SLOT(processBeatAck_v1()));
    connect(scanPosNetTalk, SIGNAL(recvConsumeAck_v1()), this, SLOT(processConsumeAck_v1()));
    scanPosNetTalk->start();

    // 微信在线支付
    connect(scanPosNetTalk, SIGNAL(recvWeixinBeatAck_v1()), this, SLOT(processWeixinBeatAck_v1()));
    connect(scanPosNetTalk, SIGNAL(recvWeixinTradeAck_v1()), this, SLOT(processWeixinTradeAck_v1()));
    // Alipay
    connect(scanPosNetTalk, SIGNAL(recvAlipayTradeAck_v1()), this, SLOT(processAlipayTradeAck_v1()));
    connect(scanPosNetTalk, SIGNAL(recvAlipayKeyInfo_v1()), this, SLOT(processAlipayKeyInfo_v1()));
    connect(scanPosNetTalk, SIGNAL(recvAlipayBlackInfo_v1()), this, SLOT(processAlipayBlackInfo_v2()));
    // driver sign
    connect(scanPosNetTalk, SIGNAL(recvDriverSignInfo_v1()), this, SLOT(processDriverSignAck_v1()));
    // pos param
    connect(scanPosNetTalk, SIGNAL(recvPosParamsetInfo()), this, SLOT(processPosParamSet()));
    connect(scanPosNetTalk, SIGNAL(recvFtpInfo()), this, SLOT(processPosFtpInfo()));
    // unionpay
    connect(scanPosNetTalk, SIGNAL(recvUnionpayAck_v1(unionPayAck_t)),this,SLOT(processUnionpayAck_v1(unionPayAck_t)));
    // up
    connect(scanPosNetTalk, SIGNAL(recvUpdataInfoAck()), this, SLOT(processUpdataInfoAck()));
#endif

    // 初始化GPS串口
    scanPosGpsTalk = new gpsSerialTalk;
//    scanPosGpsTalk->start();

    // 初始化数据库
    scanPosDB = new mySqlite;
    scanPosDB->setAbnormalOuttime(abnormalOuttime);
    scanPosDB->setTradeOuttime(tradeOuttime);
    scanPosDB->setDriverSignOuttime(driverSignOuttime);
    scanPosDB->clearAlipayOutAbnormalQR();
    //scanPosDB->clearAlipayOutTradeInfo();
    scanPosDB->clearAlipayOutTradeInfoV2(); // v2 --- 2017.3.10
    scanPosDB->clearDriverSignInfo();
    scanPosDB->initDriverSignInfo(driverID);
    scanPosTalk->setdriverID(driverID);
    scanPosDB->clearUnionpayInfo();

    // 初始化管理界面（签到、黑名单清除、设置POSID BUSID BUSLINE TICKETS）
    scanPosSignal = new signalWidget();

    scanPosBusID = new busidWidget();
    scanPosBusID->setBusID(busID);
    connect(scanPosBusID, SIGNAL(checkConfirm()), this, SLOT(processCheckConfirmBusID()));

    scanPosBusline = new buslineWidget();
    scanPosBusline->setBusline(buslineID);
    scanPosBusline->setBusticket(tickets);
    connect(scanPosBusline, SIGNAL(checkConfirm()), this, SLOT(processCheckConfirmBusline()));

    scanPosBlackClear = new blackClearWidget();

    scanPosPosID = new posidWidget();
    scanPosPosID->setPosID(posID);
    connect(scanPosPosID, SIGNAL(checkConfirm()), this, SLOT(processCheckConfirmPosID()));

    ui->myStackedWidget->addWidget(scanPosSignal);
    ui->myStackedWidget->addWidget(scanPosBusID);
    ui->myStackedWidget->addWidget(scanPosBusline);
    ui->myStackedWidget->addWidget(scanPosBlackClear);
    ui->myStackedWidget->addWidget(scanPosPosID);

    ui->myStackedWidget->addWidget(scanPosTalk);
    ui->myStackedWidget->addWidget(scanPosTradeTalk);
    ui->myStackedWidget->setCurrentWidget(scanPosTalk);

    tradeRecord = 0;
    onlineTradeRecord = 0;
    onlineScanInfoNum = 0;
    myui = signalUI;
    isClearBlankBlack = false;
    isOnlineTradeIdle = true;

    this->initAlipayKey();
    this->initAlipayBlack();
    this->initAlipayCardType();

    //新建一个QTimer对象
    myTimer = new QTimer();
    //设置定时器每个多少毫秒发送一个timeout()信号
    myTimer->setInterval(33*1000);
    //启动定时器
    myTimer->stop();
    connect(myTimer, SIGNAL(timeout()), this, SLOT(onTimerOut()));

    //新建一个QTimer对象
    failTimer = new QTimer();
    //设置定时器每个多少毫秒发送一个timeout()信号
    failTimer->setInterval(1*1000);
    //启动定时器
    failTimer->stop();
    connect(failTimer, SIGNAL(timeout()), this, SLOT(failTimerOut()));

    //新建一个QTimer对象
    tradeScanTimer = new QTimer();
    //设置定时器每个多少毫秒发送一个timeout()信号
    tradeScanTimer->setInterval(60*1000);
    //启动定时器
    tradeScanTimer->start();
    //connect(tradeScanTimer, SIGNAL(timeout()), this, SLOT(tradeScanTimerOut()));
    connect(tradeScanTimer, SIGNAL(timeout()), this, SLOT(tradeScanTimerOutV2()));    // v2 --- 2017.3.10

    //新建一个QTimer对象
    signScanTimer = new QTimer();
    //设置定时器每个多少毫秒发送一个timeout()信号
    signScanTimer->setInterval(2*60*1000);
    //启动定时器
    signScanTimer->start();
    connect(signScanTimer, SIGNAL(timeout()), this, SLOT(signScanTimerOut()));

    //新建一个QTimer对象
    onlineValidTimer = new QTimer();
    //设置定时器每个多少毫秒发送一个timeout()信号
    onlineValidTimer->setInterval(3*1000);
    //暂停定时器
    onlineValidTimer->stop();
    connect(onlineValidTimer, SIGNAL(timeout()), this, SLOT(onlineValidTimerOut()));

    isAskKey = true;
    isAskBlack = true;
    judgeUp - false;
    judgeFtpWork = false;
    scanPosScanTalk->tradeRledCtrl(0);
    scanPosScanTalk->tradeGledCtrl(0);
}

myPosWidget::~myPosWidget()
{
    delete ui;
}

void myPosWidget::onTimerOut()
{
    if(!isAskKey)
        isAskKey = true;

    if(!isAskBlack)
        isAskBlack = true;
}

void myPosWidget::failTimerOut()
{
    failTimer->stop();
    scanPosScanTalk->tradeRledCtrl(0);
}

void myPosWidget::tradeScanTimerOut()
{
    //unionpay record upload
    unionPayInfo_t tagUnionPay;
    memset(&tagUnionPay, 0, sizeof(unionPayInfo_t));
    if(scanPosDB->selectUnionpayInfo(tagUnionPay))
    {
        int datalen = sizeof(unionPayInfo_t);
        unsigned char tradeData[256] = {0};
        tradeData[0] = 0x55;
        tradeData[1] = 0x7a;
        if(tagUnionPay.tradeNature==0)
            tradeData[2] = 0x36;
        else
            tradeData[2] = 0x37;
        memcpy(&tradeData[3], &datalen, 2);
        tradeData[5] = 0x00;
        memcpy(&tradeData[6],&tagUnionPay,sizeof(unionPayInfo_t));
        unsigned char check = 0;
        for(int i=0; i<datalen+6; i++)
            check ^= tradeData[i];
        tradeData[datalen+6] = check;
        int sendlen;
        int ret = scanPosNetTalk->socketSend(tradeData, 7+datalen, &sendlen);
        if(1 == ret)
        {
            //scanPosStmTalk->stmVoiceCmd(10);
            qDebug("qwy --- unionpay trade 发送交易记录失败...");
        }
    }else{
         qDebug("qwy --- all unionpay trade record have send...");
    }

    //if(scanPosGprsTalk->gprsInit == finishINIT)
    {
        alipayTradeInfo_t salipayTradeInfo;
        memset(&salipayTradeInfo, 0, sizeof(alipayTradeInfo_t));
        bool scanret = scanPosDB->scanAlipayTradeRecordInfo(salipayTradeInfo);
        if(false == scanret)
        {
            qDebug("kira --- all alipay trade record have send...");
            return;
        }
        int uidLen;
        int recordLen;
        memcpy(&uidLen, salipayTradeInfo.offlineHeadInfo.userIDLen, 4);
        memcpy(&recordLen, salipayTradeInfo.offlineHeadInfo.offLineRecordLen, 4);
        // 组包
        int datalen = sizeof(offlineHeadInfo_t) + uidLen + recordLen;

        unsigned char tradeData[4096];
        memset(tradeData, 0, 4096);
        tradeData[0] = 0x55;
        tradeData[1] = 0x7a;
        tradeData[2] = 0x24;
        memcpy(&tradeData[3], &datalen, 2);
        tradeData[5] = 0x00;
        memcpy(&tradeData[6],
                &salipayTradeInfo.offlineHeadInfo,
                sizeof(offlineHeadInfo_t));
        memcpy(&tradeData[6+sizeof(offlineHeadInfo_t)],
                salipayTradeInfo.userID, uidLen);
        memcpy(&tradeData[6+sizeof(offlineHeadInfo_t)+uidLen],
                salipayTradeInfo.record, recordLen);
        qDebug("kira --- check trade scan ready send information...");
        unsigned char check = 0;
        for(int i=0; i<datalen+6; i++)
            check ^= tradeData[i];
        tradeData[datalen+6] = check;

#if 0
        printf("kira --- scan tradeDataInfoV2: \n");
        for(int x=0; x<datalen+7; x++)
            printf("%.2x ", tradeData[x]);
        printf("\n");
#endif

        //int ret = scanPosGprsTalk->sendGprsData((char*)tradeData, 7+datalen);
        int sendlen;
        int ret = scanPosNetTalk->socketSend(tradeData, 7+datalen, &sendlen);
        if(1 == ret)
        {
            //scanPosStmTalk->stmVoiceCmd(10);
            qDebug("kira --- trade scan 发送交易记录失败...");
        }
    }
}

// v2 --- 2017.3.10
void myPosWidget::tradeScanTimerOutV2()
{
    //unionpay record upload
    unionPayInfo_t tagUnionPay;
    memset(&tagUnionPay, 0, sizeof(unionPayInfo_t));
    if(scanPosDB->selectUnionpayInfo(tagUnionPay))
    {
        int datalen = sizeof(unionPayInfo_t);
        unsigned char tradeData[256] = {0};
        tradeData[0] = 0x55;
        tradeData[1] = 0x7a;
        if(tagUnionPay.tradeNature==0)
            tradeData[2] = 0x36;
        else
            tradeData[2] = 0x37;
        memcpy(&tradeData[3], &datalen, 2);
        tradeData[5] = 0x00;
        memcpy(&tradeData[6],&tagUnionPay,sizeof(unionPayInfo_t));
        unsigned char check = 0;
        for(int i=0; i<datalen+6; i++)
            check ^= tradeData[i];
        tradeData[datalen+6] = check;
        int sendlen;
        int ret = scanPosNetTalk->socketSend(tradeData, 7+datalen, &sendlen);
        if(1 == ret)
        {
            //scanPosStmTalk->stmVoiceCmd(10);
            qDebug("qwy --- unionpay trade 发送交易记录失败...");
        }
    }else{
         qDebug("qwy --- all unionpay trade record have send...");
    }

    //if(scanPosGprsTalk->gprsInit == finishINIT)
    {
        alipayTradeInfoV2_t salipayTradeInfo;
        memset(&salipayTradeInfo, 0, sizeof(alipayTradeInfoV2_t));
        bool scanret = scanPosDB->scanAlipayTradeRecordInfoV2(salipayTradeInfo);
        if(false == scanret)
        {
            qDebug("kira --- all alipayV2 trade record have send...");
            return;
        }
        int uidLen;
        int recordLen;
        int cardNoLen;
        int cardDataLen;
        int cardTypeLen;
        memcpy(&uidLen, salipayTradeInfo.offlineHeadInfo.userIDLen, 4);
        memcpy(&recordLen, salipayTradeInfo.offlineHeadInfo.offLineRecordLen, 4);
        memcpy(&cardNoLen, salipayTradeInfo.offlineHeadInfo.cardNoLen, 4);
        memcpy(&cardDataLen, salipayTradeInfo.offlineHeadInfo.cardDataLen, 4);
        memcpy(&cardTypeLen, salipayTradeInfo.offlineHeadInfo.cardTypeLen, 4);

        // 组包
        int datalen = sizeof(offlineHeadInfoV2_t) + uidLen + recordLen
                        + cardNoLen + cardDataLen + cardTypeLen;

        unsigned char tradeData[4096];
        memset(tradeData, 0, 4096);
        tradeData[0] = 0x55;
        tradeData[1] = 0x7a;
        tradeData[2] = 0x39;
        memcpy(&tradeData[3], &datalen, 2);
        tradeData[5] = 0x00;
        memcpy(&tradeData[6],
                &salipayTradeInfo.offlineHeadInfo,
                sizeof(offlineHeadInfoV2_t));
        memcpy(&tradeData[6+sizeof(offlineHeadInfoV2_t)],
                salipayTradeInfo.userID, uidLen);
        memcpy(&tradeData[6+sizeof(offlineHeadInfoV2_t)+uidLen],
                salipayTradeInfo.record, recordLen);
        memcpy(&tradeData[6+sizeof(offlineHeadInfoV2_t)+uidLen+recordLen],
                salipayTradeInfo.cardNo, cardNoLen);
        memcpy(&tradeData[6+sizeof(offlineHeadInfoV2_t)+uidLen+recordLen+cardNoLen],
                salipayTradeInfo.cardData, cardDataLen);
        memcpy(&tradeData[6+sizeof(offlineHeadInfoV2_t)+uidLen+recordLen+cardNoLen+cardTypeLen],
                salipayTradeInfo.record, cardTypeLen);
        qDebug("kira --- check trade scan ready send information...");
        unsigned char check = 0;
        for(int i=0; i<datalen+6; i++)
            check ^= tradeData[i];
        tradeData[datalen+6] = check;

#if 0
        printf("kira --- scan tradeDataInfo: \n");
        for(int x=0; x<datalen+7; x++)
            printf("%.2x ", tradeData[x]);
        printf("\n");
#endif

        //int ret = scanPosGprsTalk->sendGprsData((char*)tradeData, 7+datalen);
        int sendlen;
        int ret = scanPosNetTalk->socketSend(tradeData, 7+datalen, &sendlen);
        if(1 == ret)
        {
            //scanPosStmTalk->stmVoiceCmd(10);
            qDebug("kira --- trade scan 发送交易记录失败...");
        }
    }
}

void myPosWidget::signScanTimerOut()
{
    //if(scanPosGprsTalk->gprsInit == finishINIT)
    {
        driveSignNetInfo_t sdriveSignNetInfo;
        memset(&sdriveSignNetInfo, 0, sizeof(driveSignNetInfo_t));
        bool scanret = scanPosDB->scanDriverSignInfo(sdriveSignNetInfo);
        if(false == scanret)
        {
            qDebug("kira --- all driver sign record have send...");
            return;
        }

        // 组包
        int datalen = sizeof(driveSignNetInfo_t);
        unsigned char signData[1024] = {0};
        memset(signData, 0, sizeof(signData));
        signData[0] = 0x55;
        signData[1] = 0x7a;
        signData[2] = 0x29;
        memcpy(&signData[3], &datalen, 2);
        signData[5] = 0x00;
        memcpy(&signData[6], &sdriveSignNetInfo, datalen);
        unsigned char check = 0;
        for(int i=0; i<datalen+6; i++)
            check ^= signData[i];
        signData[datalen+6] = check;

        //int ret = scanPosGprsTalk->sendGprsData((char*)signData, 7+datalen);
        int sendlen;
        int ret = scanPosNetTalk->socketSend(signData, 7+datalen, &sendlen);
        if(1 == ret)
        {
            qDebug("kira --- 发送司机签到记录失败...");
        }
    }
}

void myPosWidget::showTradeDialog()
{
    // 测试用
    //balance = "120.00";
    //debit = "2.00";

    //int tradeRet = processTrade();
    //int tradeRet = processAlipayScanInfo();
    int tradeRet = processAlipayScanInfoV2();
    if(tradeRet == 0)
    {
        scanPosStmTalk->setBalance(balance);
        scanPosStmTalk->setDebit(debit);
        //scanPosStmTalk->stmVoiceCmd(12);
        scanPosStmTalk->stmVoiceCmd(26);

        scanPosTradeTalk->setBalance(balance);
        scanPosTradeTalk->setDebit(debit);

        scanPosScanTalk->tradeGledCtrl(1);
        if(isShowPosWidget)
        {
            isShowPosWidget = false;
            ui->myStackedWidget->setCurrentWidget(scanPosTradeTalk);
            //启动定时器
            scanPosTradeTalk->myTimer->start();
        }
        else
        {
            //重置定时器
            scanPosTradeTalk->myTimer->stop();
            scanPosTradeTalk->myTimer->start();
        }
    }
    else if(tradeRet == 3)
    {
        scanPosStmTalk->stmVoiceCmd(2);
    }
    else if((tradeRet == 2) || (tradeRet == 1))
    {
        //scanPosStmTalk->stmVoiceCmd(19);
        scanPosStmTalk->stmVoiceCmd(27);
    }
    else if(tradeRet == 5)
    {
        scanPosStmTalk->stmVoiceCmd(17);
        //scanPosStmTalk->stmVoiceCmd(27);
    }
    else if(tradeRet == 7)
    {
        ;
    }
}

void myPosWidget::showPosWidget()
{
    scanPosTradeTalk->myTimer->stop();

    isShowPosWidget = true;
    ui->myStackedWidget->setCurrentWidget(scanPosTalk);

    scanPosScanTalk->tradeGledCtrl(0);
    // 测试用
    //balance = "2.00";
    //debit = "120.00";
}

void myPosWidget::InitialConfig()
{
    configFile = "/opt/kira/configure/setting.ini";
    servergroup = "server";
    workgroup = "work";

    Aliapygroup = "Alipay";
    dbgroup = "DB";

    // 网络
    configSet.readSetInfo(configFile, servergroup, "serverIP", serverIP);
    configSet.readSetInfo(configFile, servergroup, "serverPort", serverPort);
    configSet.readSetInfo(configFile, servergroup, "beatInterval", beatInterval);

    // 运营
    configSet.readSetInfo(configFile, workgroup, "busID", busID);
    configSet.readSetInfo(configFile, workgroup, "buslineID", buslineID);
    configSet.readSetInfo(configFile, workgroup, "posID", posID);
    configSet.readSetInfo(configFile, workgroup, "tickets", tickets);
    configSet.readSetInfo(configFile, workgroup, "monthrate", monthrate);
    configSet.readSetInfo(configFile, workgroup, "walletrate", walletrate);
    configSet.readSetInfo(configFile, workgroup, "LVersion", LVersion); // kira added - 2017.3.7
    configSet.readSetInfo(configFile, workgroup, "SVersion", SVersion);
    configSet.readSetInfo(configFile, workgroup, "LTempVersion", LTempVersion); // kira added - 2017.3.14
    configSet.readSetInfo(configFile, workgroup, "STempVersion", STempVersion);

    if(access("/opt/kira/updata/newUpdate.notify", F_OK) == 0)
    {
        judgeUp = true;
        qDebug("kira --- program has just updata");
        LVersion = LTempVersion;
        configSet.writeSetInfo(configFile, workgroup, "LVersion", LVersion);
    }
    else
    {
        //configSet.writeSetInfo(configFile, workgroup, "LTempVersion", LVersion);
        //configSet.writeSetInfo(configFile, workgroup, "STempVersion", SVersion);
    }

    // 微信在线支付
    configSet.readSetInfo(configFile, workgroup, "cityID", cityID);

    // Alipay
    configSet.readSetInfo(configFile, Aliapygroup, "blacklistVer", blacklistVer);
    configSet.readSetInfo(configFile, Aliapygroup, "keylistVer", keylistVer);
    configSet.readSetInfo(configFile, Aliapygroup, "blackCurrentNum", blackCurrentNum);
    configSet.readSetInfo(configFile, Aliapygroup, "keyCurrentNum", keyCurrentNum);

    //DB
    configSet.readSetInfo(configFile, dbgroup, "abnormalOuttime", abnormalOuttime);
    configSet.readSetInfo(configFile, dbgroup, "tradeOuttime", tradeOuttime);
    configSet.readSetInfo(configFile, dbgroup, "driverSignOuttime", driverSignOuttime);

}

//
int myPosWidget::processTrade()
{
    scanTradeInfo tradeInfo;
    consumeInfo_t consumeInfo;
    scanPosScanTalk->getScanInfo(&tradeInfo);

    if(memcmp(tradeInfo.framehead, "ebus://", strlen("ebus://")) == 0)
    {
        QString dateTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
        QString tradeDateTime = QByteArray::fromRawData((char*)tradeInfo.QRvalidTime, sizeof(tradeInfo.QRvalidTime));
        // 二维码有效性判断（有效时间）
        double dateTimeDiff1 = dateTime.toDouble() - tradeDateTime.toDouble();
        double dateTimeDiff2 = tradeDateTime.toDouble() - dateTime.toDouble();
        if((dateTimeDiff1 > MAX_DATETIMEDIFF) || (dateTimeDiff2 > MAX_DATETIMEDIFF))
        {
            qDebug("kira --- This two-dimensional code is invalid");
            return 2;
        }

        // 二维码重复扫码判断（生成时间+用户ID）
        tradeBalance_t mytradeBalance;
        bool judgeRepeatScanRet = scanPosDB->judgeRepeatScan(tradeInfo, &mytradeBalance);
        if(judgeRepeatScanRet == true)
        {
            qDebug("kira --- This two-dimensional code is repeat scan");
            return 3;
        }

        qDebug("kira --- balance info: wallet=%d month=%d", mytradeBalance.wallet, mytradeBalance.month);

        QByteArray id = posID.toLatin1();
        char *temp = id.data();
        memcpy(consumeInfo.deviceID, (unsigned char*)temp, sizeof(consumeInfo.deviceID));
        memcpy(consumeInfo.userID, tradeInfo.account, sizeof(tradeInfo.account));
        // 同一用户月票区10分钟之内重复扫码判断
        bool judgeRepeatConsumeRet = scanPosDB->judgeRepeatConsume(tradeInfo, &mytradeBalance);
        if((tradeInfo.accountState == '0') || (true == judgeRepeatConsumeRet))
        {
            consumeInfo.tradeType = WALLETTRADE;
            qDebug("kira --- walletrate = %d", walletrate.toInt());
            int amount = tickets.toDouble()*walletrate.toInt();
            memcpy(consumeInfo.amount, &amount, 2);

            QString amountInteger = QString::number(amount/100, 10);
            QString amountDecimal = QString::number(amount%100, 10);
            debit.clear();
            debit.append(amountInteger);
            debit.append(".");
            debit.append(amountDecimal);

            QString strBlance = QByteArray::fromRawData((char*)tradeInfo.walletBalance, sizeof(tradeInfo.walletBalance));
            int intBalance = strBlance.toInt();
            QString balanceInteger = QString::number(intBalance/100, 10);
            QString balanceDecimal = QString::number(intBalance%100, 10);
            balance.clear();
            balance.append(balanceInteger);
            balance.append(".");
            balance.append(balanceDecimal);
            // 判断钱包区是否有余额
            if(intBalance <= amount)
            {
                if(mytradeBalance.walletRepeat && (mytradeBalance.wallet <= amount))
                {
                    qDebug("kira --- Insufficient user wallet balance");
                    return 4;
                }
            }
        }
        else
        {         
            consumeInfo.tradeType = MONTHTRADE;
            qDebug("kira --- monthrate = %d", monthrate.toInt());
            int amount = tickets.toDouble()*monthrate.toInt();
            memcpy(consumeInfo.amount, &amount, 2);

            QString amountInteger = QString::number(amount/100, 10);
            QString amountDecimal = QString::number(amount%100, 10);
            debit.clear();
            debit.append(amountInteger);
            debit.append(".");
            debit.append(amountDecimal);

            QString strBlance = QByteArray::fromRawData((char*)tradeInfo.monthBalance, sizeof(tradeInfo.monthBalance));
            int intBalance = strBlance.toInt();
            QString balanceInteger = QString::number(intBalance/100, 10);
            QString balanceDecimal = QString::number(intBalance%100, 10);
            balance.clear();
            balance.append(balanceInteger);
            balance.append(".");
            balance.append(balanceDecimal);
            // 判断月票区是否有余额
            if(intBalance <= amount)
            {
                if(mytradeBalance.monthRepeat && (mytradeBalance.month <= amount))
                {
                    qDebug("kira --- Insufficient user month balance");
                    return 4;
                }
            }
        }
        memcpy(consumeInfo.citycode, tradeInfo.citycode, sizeof(tradeInfo.citycode));
        for(int p=0; p<sizeof(tradeInfo.QRgenerationTime)/2; p++)
        {
            consumeInfo.tradeTime[p] =
                    (unsigned char)(((AscToHex(tradeInfo.QRgenerationTime[2*p])<<4) & 0xf0)
                                    +(AscToHex(tradeInfo.QRgenerationTime[2*p+1]) & 0x0f));
        }
        int ticketPrice = (int)(tickets.toDouble()*100.0);
        qDebug("kira --- tickets = %lf", tickets.toDouble());
        memcpy(consumeInfo.ticketPrice, &ticketPrice, 2);
        // location fill test data
        memcpy(consumeInfo.longitude, "120.123E", sizeof(consumeInfo.longitude));
        memcpy(consumeInfo.latitude, "30.3452N", sizeof(consumeInfo.latitude));

        unsigned char tradeData[7+sizeof(consumeInfo_t)] = {0};
        tradeData[0] = 0x55;
        tradeData[1] = 0x7a;
        tradeData[2] = 0x02;
        int consumeInfoLen = sizeof(consumeInfo_t);
        memcpy(&tradeData[3], &consumeInfoLen, 2);
        tradeData[5] = 0x00;
        memcpy(&tradeData[6], &consumeInfo, sizeof(consumeInfo_t));
        unsigned char check = 0;
        for(int i=0; i<sizeof(consumeInfo_t)+6; i++)
            check ^= tradeData[i];
        tradeData[sizeof(consumeInfo_t)+6] = check;
        // 保存tradeInfo consumeInfo到相应的表空间
        bool insertRet = insertTransactionData(tradeInfo, consumeInfo);
        if(insertRet == false)
        {
            qDebug("kira --- insert data failed ");
            return 5;
        }

        //scanPosGprsTalk->sendGprsData((char*)tradeData, sizeof(tradeData));
        int sendlen;
        scanPosNetTalk->socketSend(tradeData, sizeof(tradeData), &sendlen);
        return 0;
    }
    else
    {
        qDebug("kira --- This two-dimensional code does not contain EBUS");
        return 1;
    }
}

unsigned char myPosWidget::AscToHex(unsigned char aChar)
{
    if((aChar>=0x30)&&(aChar<=0x39))
        aChar -= 0x30;
    else if((aChar>=0x41)&&(aChar<=0x46))//大写字母
        aChar -= 0x37;
    else if((aChar>=0x61)&&(aChar<=0x66))//小写字母
        aChar -= 0x57;
    else
        aChar = 0xff;
    return aChar;
}

unsigned char myPosWidget::HexToAsc(unsigned char aHex)
{
    unsigned char  ASCII_Data;
    ASCII_Data = aHex & 0x0f;
    if(ASCII_Data < 10)
        ASCII_Data = ASCII_Data + 0x30;         //‘0--9’
    else
        ASCII_Data = ASCII_Data + 0x37;         //‘A--F’
    return ASCII_Data;
}

bool myPosWidget::insertTransactionData(scanTradeInfo tradeInfo, consumeInfo_t consumeInfo)
{
    // 需要添加互斥锁

    dbConsumeInfo_t dbConsumeInfo;
    memcpy(dbConsumeInfo.deviceID, consumeInfo.deviceID, sizeof(consumeInfo.deviceID));
    memcpy(dbConsumeInfo.userID, consumeInfo.userID, sizeof(consumeInfo.userID));
    dbConsumeInfo.tradeType = consumeInfo.tradeType;
    memcpy(dbConsumeInfo.citycode, consumeInfo.citycode, sizeof(consumeInfo.citycode));
    memcpy(dbConsumeInfo.tradeTime, tradeInfo.QRgenerationTime, sizeof(tradeInfo.QRgenerationTime));
    memcpy(dbConsumeInfo.longitude, consumeInfo.longitude, sizeof(consumeInfo.longitude));
    memcpy(dbConsumeInfo.latitude, consumeInfo.latitude, sizeof(consumeInfo.latitude));

    int ticketPrice = (int)(tickets.toDouble()*100.0);
    dbConsumeInfo.ticketPrice[0] = '0' + ticketPrice/1000;
    dbConsumeInfo.ticketPrice[1] = '0' + (ticketPrice%1000)/100;
    dbConsumeInfo.ticketPrice[2] = '0' + (ticketPrice%100)/10;
    dbConsumeInfo.ticketPrice[3] = '0' + ticketPrice%10;
    int amount;
    if(dbConsumeInfo.tradeType == WALLETTRADE)
    {
        amount = tickets.toDouble()*walletrate.toInt();
    }
    else
    {
        amount = tickets.toDouble()*monthrate.toInt();
    }
    dbConsumeInfo.amount[0] = '0' + amount/1000;
    dbConsumeInfo.amount[1] = '0' + (amount%1000)/100;
    dbConsumeInfo.amount[2] = '0' + (amount%100)/10;
    dbConsumeInfo.amount[3] = '0' + amount%10;

    bool insertRet;
    insertRet = scanPosDB->insertTradeData(tradeInfo);
    if(false == insertRet)
        return insertRet;
    insertRet = scanPosDB->insertConsumeData(dbConsumeInfo);
    return insertRet;
}

void myPosWidget::processBeatInfo()
{
    beatInfo_t beatInfo;
    int sendlen;
    unsigned char beatData[7+sizeof(beatInfo_t)] = {0};
    beatData[0] = 0x55;
    beatData[1] = 0x7a;
    beatData[2] = 0x01;
    int beatInfoLen = sizeof(beatInfo_t);
    memcpy(&beatData[3], &beatInfoLen, 2);
    beatData[5] = 0x00;
    QByteArray id = posID.toLatin1();
    char *temp = id.data();
    memcpy(beatInfo.deviceID, (unsigned char*)temp, sizeof(beatInfo.deviceID));
    memcpy(&beatData[6], &beatInfo, sizeof(beatInfo_t));
    unsigned char check = 0;
    for(int i=0; i<sizeof(beatInfo_t)+6; i++)
        check ^= beatData[i];
    beatData[sizeof(beatInfo_t)+6] = check;
    //scanPosGprsTalk->sendGprsData((char*)beatData, sizeof(beatData));
    //QString beatStr = QByteArray::fromRawData((char*)beatData, beatInfoLen);
#if 1
    printf("kira --- beatData: ");
    for(int i=0; i<beatInfoLen+7; i++)
        printf("%.2x", beatData[i]);
    printf("\n");
#endif
    scanPosNetTalk->socketSend(beatData, sizeof(beatData), &sendlen);
}

void myPosWidget::processBeatAck_v1()
{
    beatAckInfo_t beatAckInfo;
    //scanPosGprsTalk->readBeatAckInfo(&beatAckInfo);
    scanPosNetTalk->readBeatAckInfo(&beatAckInfo);
    keyBatch = QByteArray::fromRawData((char*)beatAckInfo.keyBatch, sizeof(beatAckInfo.keyBatch));

    QString currentDateTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    unsigned char dateTime[14];
    for(int i=0; i<7; i++)
    {
        unsigned char dateTimeL = beatAckInfo.checkTime[i] & 0x0f;
        unsigned char dateTimeH = (beatAckInfo.checkTime[i]>>4) & 0x0f;
        dateTime[2*i] = HexToAsc(dateTimeH);
        dateTime[2*i+1] = HexToAsc(dateTimeL);
    }
    QString serverDateTime = QByteArray::fromRawData((char*)dateTime, sizeof(dateTime));

    double dateTimeDiff1 = currentDateTime.toDouble() - serverDateTime.toDouble();
    double dateTimeDiff2 = serverDateTime.toDouble() - currentDateTime.toDouble();
    if((dateTimeDiff1 > MAX_BEATTIMEDIFF) || (dateTimeDiff2 > MAX_BEATTIMEDIFF))
    {
        qDebug("kira --- use server datetime check system datetime...");
        char cmdData[60] = {0};
        sprintf(cmdData, "date %.2x%.2x.%.2x.%.2x-%.2x:%.2x:%.2x",
                beatAckInfo.checkTime[0], beatAckInfo.checkTime[1], beatAckInfo.checkTime[2],
                beatAckInfo.checkTime[3], beatAckInfo.checkTime[4], beatAckInfo.checkTime[5],
                beatAckInfo.checkTime[6]);
        system(cmdData);
        system("hwclock -w");
    }
}

void myPosWidget::processConsumeAck_v1()
{
    consumeAckInfo_t consumeAckInfo;
    //scanPosGprsTalk->readConsumeAckInfo(&consumeAckInfo);
    scanPosNetTalk->readConsumeAckInfo(&consumeAckInfo);
    unsigned char dateTime[14];
    for(int i=0; i<7; i++)
    {
        unsigned char dateTimeL = consumeAckInfo.time[i] & 0x0f;
        unsigned char dateTimeH = (consumeAckInfo.time[i]>>4) & 0x0f;
        dateTime[2*i] = HexToAsc(dateTimeH);
        dateTime[2*i+1] = HexToAsc(dateTimeL);
    }
    QString tradeTime = QByteArray::fromRawData((char*)dateTime, sizeof(dateTime));
    QString userID = QByteArray::fromRawData((char*)consumeAckInfo.userID, sizeof(consumeAckInfo.userID));

    bool updataRet = scanPosDB->updataServerAck(userID, tradeTime);
    if(false == updataRet)
        qDebug("kira --- updata serverAck failed...");
}

// 微信在线支付
void myPosWidget::processWeixinScanInfo()
{
    weixinScanInfo_t weixinScanInfo;
    onlineTradeHeadInfo_t onlineTradeHeadInfo;
    scanPosScanTalk->getWeixinScanInfo(&weixinScanInfo);
    unsigned char flag = ((AscToHex(weixinScanInfo.scanData[0]) << 4) & 0xf0) + (AscToHex(weixinScanInfo.scanData[1]) & 0x0f);;

    if(false == isOnlineTradeIdle)
    {
        //scanPosStmTalk->stmVoiceCmd(2);
        return;
    }
    isOnlineTradeIdle = false; //bug

    if((flag == 0x10) || (flag == 0x11) || (flag == 0x12) || (flag == 0x13) || (flag == 0x14) || (flag == 0x15) || (flag == 0x28) || (flag == 0x83))
    {        
        QString onlineScanInfoStr = QByteArray::fromRawData((char*)weixinScanInfo.scanData, weixinScanInfo.scanLen);

        for(QMap<int, QString>::iterator it = onlineScanInfoMap.begin(); it != onlineScanInfoMap.end(); it++)
        {
            if(it.value() == onlineScanInfoStr)
            {
                qDebug("kira --- online same QR");
                isOnlineTradeIdle = true;
                return;
            }
        }

        memcpy(onlineTradeHeadInfo.deviceID, (unsigned char*)posID.toLatin1().data(), sizeof(onlineTradeHeadInfo.deviceID));
        memcpy(onlineTradeHeadInfo.citycode, (unsigned char*)cityID.toLatin1().data(), sizeof(onlineTradeHeadInfo.citycode));
        memcpy(onlineTradeHeadInfo.buslineID, (unsigned char*)buslineID.toLatin1().data(), sizeof(onlineTradeHeadInfo.buslineID));
        memcpy(onlineTradeHeadInfo.busID, (unsigned char*)busID.toLatin1().data(), sizeof(onlineTradeHeadInfo.busID));
        memcpy(onlineTradeHeadInfo.driverID, (unsigned char*)driverID.toLatin1().data(), sizeof(onlineTradeHeadInfo.driverID));

        QString currentDateTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
        unsigned char datetime[14];
        memcpy(datetime, (unsigned char*)currentDateTime.toLatin1().data(), currentDateTime.length());
        for(int i=0; i<7; i++)
        {
            onlineTradeHeadInfo.tradeTime[i] = ((AscToHex(datetime[2*i]) << 4) & 0xf0) + (AscToHex(datetime[2*i+1]) & 0x0f);
        }

        memcpy(onlineTradeHeadInfo.serialNum, &onlineTradeRecord, 4);

        if(onlineTradeRecord++ > 9000)
            onlineTradeRecord = 0;

        onlineTradeHeadInfo.validTime = 10;

        int ticketPrice = (int)(tickets.toDouble()*100.0);
        qDebug("kira --- online tickets = %lf", tickets.toDouble());
        memcpy(onlineTradeHeadInfo.ticketPrice, &ticketPrice, 2);

        qDebug("kira --- online walletrate = %d", walletrate.toInt());
        int amount = tickets.toDouble()*walletrate.toInt();
        memcpy(onlineTradeHeadInfo.amount, &amount, 2);
        QString amountInteger = QString::number(amount/100, 10);
        QString amountDecimal = QString::number(amount%100, 10);
        debit.clear();
        debit.append(amountInteger);
        debit.append(".");
        debit.append(amountDecimal);

        int intBalance = 0;
        QString balanceInteger = QString::number(intBalance/100, 10);
        QString balanceDecimal = QString::number(intBalance%100, 10);
        balance.clear();
        balance.append(balanceInteger);
        balance.append(".");
        balance.append(balanceDecimal);

        // location fill test data
        memcpy(onlineTradeHeadInfo.longitude, "120.123E", sizeof(onlineTradeHeadInfo.longitude));
        memcpy(onlineTradeHeadInfo.latitude, "30.3452N", sizeof(onlineTradeHeadInfo.latitude));

        unsigned char tradeData[7+sizeof(onlineTradeHeadInfo_t)+MAX_WEIXIN_DATALEN] = {0};
        tradeData[0] = 0x55;
        tradeData[1] = 0x7a;
        tradeData[2] = 0x22;
        int tradeInfoLen = sizeof(onlineTradeHeadInfo_t)+weixinScanInfo.scanLen;
        memcpy(&tradeData[3], &tradeInfoLen, 2);
        tradeData[5] = 0x00;
        memcpy(&tradeData[6], &onlineTradeHeadInfo, sizeof(onlineTradeHeadInfo_t));
        memcpy(&tradeData[6+sizeof(onlineTradeHeadInfo_t)], weixinScanInfo.scanData, weixinScanInfo.scanLen);
        unsigned char check = 0;
        for(int i=0; i<tradeInfoLen+6; i++)
            check ^= tradeData[i];
        tradeData[tradeInfoLen+6] = check;

        //int ret = scanPosGprsTalk->sendGprsData((char*)tradeData, tradeInfoLen+7);
        int sendlen;
        int ret = scanPosNetTalk->socketSend(tradeData, tradeInfoLen+7, &sendlen);
        if(1 == ret)
        {
            scanPosStmTalk->stmVoiceCmd(10);
            isOnlineTradeIdle = true;
        }
        else
        {
            onlineValidTimer->start();
            onlineScanInfoMap.insert(onlineScanInfoNum, onlineScanInfoStr);
            if(onlineScanInfoNum++ > 100)
                onlineScanInfoNum = 0;
        }
    }
    else
    {
        scanPosStmTalk->stmVoiceCmd(27);
        isOnlineTradeIdle = true;
    }
}

void myPosWidget::onlineValidTimerOut()
{
    onlineValidTimer->stop();
    scanPosStmTalk->stmVoiceCmd(9);
    isOnlineTradeIdle = true;
}

#if 0
void myPosWidget::processWeixinScanInfo()
{
    weixinScanInfo_t weixinScanInfo;
    weixinTradeInfo_t weixinTradeInfo;
    scanPosScanTalk->getWeixinScanInfo(&weixinScanInfo);

    memcpy(weixinTradeInfo.scanData, weixinScanInfo.scanData, weixinScanInfo.scanLen);

    QByteArray id = posID.toLatin1();
    char *temp = id.data();
    memcpy(weixinTradeInfo.deviceID, (unsigned char*)temp, sizeof(weixinTradeInfo.deviceID));

    QByteArray id1 = cityID.toLatin1();
    char *temp1 = id1.data();
    memcpy(weixinTradeInfo.citycode, (unsigned char*)temp1, sizeof(weixinTradeInfo.citycode));

    int ticketPrice = (int)(tickets.toDouble()*100.0);
    qDebug("kira --- tickets = %lf", tickets.toDouble());
    memcpy(weixinTradeInfo.ticketPrice, &ticketPrice, 2);

    qDebug("kira --- walletrate = %d", walletrate.toInt());
    int amount = tickets.toDouble()*walletrate.toInt();
    memcpy(weixinTradeInfo.amount, &amount, 2);
    QString amountInteger = QString::number(amount/100, 10);
    QString amountDecimal = QString::number(amount%100, 10);
    debit.clear();
    debit.append(amountInteger);
    debit.append(".");
    debit.append(amountDecimal);

    int intBalance = 0;
    QString balanceInteger = QString::number(intBalance/100, 10);
    QString balanceDecimal = QString::number(intBalance%100, 10);
    balance.clear();
    balance.append(balanceInteger);
    balance.append(".");
    balance.append(balanceDecimal);

    // location fill test data
    int speed = 2000;
    memcpy(weixinTradeInfo.longitude, "120.123E", sizeof(weixinTradeInfo.longitude));
    memcpy(weixinTradeInfo.latitude, "30.3452N", sizeof(weixinTradeInfo.latitude));
    memcpy(weixinTradeInfo.speed, &speed, 2);

    unsigned char tradeData[7+sizeof(weixinTradeInfo_t)] = {0};
    tradeData[0] = 0x55;
    tradeData[1] = 0x7a;
    tradeData[2] = 0x22;
    int tradeInfoLen = sizeof(weixinTradeInfo_t);
    memcpy(&tradeData[3], &tradeInfoLen, 2);
    tradeData[5] = 0x00;
    memcpy(&tradeData[6], &weixinTradeInfo, sizeof(weixinTradeInfo_t));
    unsigned char check = 0;
    for(int i=0; i<sizeof(weixinTradeInfo_t)+6; i++)
        check ^= tradeData[i];
    tradeData[sizeof(weixinTradeInfo_t)+6] = check;

    int ret = scanPosGprsTalk->sendGprsData((char*)tradeData, sizeof(tradeData));
    if(1 == ret)
    {
        scanPosStmTalk->stmVoiceCmd(10);
    }
}
#endif

void myPosWidget::processWeixinTradeAck_v1()
{
    //weixinTradeAck_t weixinTradeAck;
    //scanPosGprsTalk->readweixinTradeAckInfo(&weixinTradeAck);
    onlineTradeAck_t onlineTradeAck;
    //scanPosGprsTalk->readOnlineTradeAckInfo(&onlineTradeAck);
    scanPosNetTalk->readOnlineTradeAckInfo(&onlineTradeAck);
    onlineValidTimer->stop();
    isOnlineTradeIdle = true;

    qDebug("kira --- onlineTradeAck.tradeRet: %d", onlineTradeAck.tradeRet);
    //if(1 == weixinTradeAck.tradeRet)
    if(0 == onlineTradeAck.tradeRet)
    {
        qDebug("kira --- onlineTrade check1");
        scanPosStmTalk->setBalance(balance);
        scanPosStmTalk->setDebit(debit);
        scanPosStmTalk->stmVoiceCmd(26);

        scanPosTradeTalk->setBalance(balance);
        scanPosTradeTalk->setDebit(debit);

        scanPosScanTalk->tradeGledCtrl(1);
        if(isShowPosWidget)
        {
            qDebug("kira --- onlineTrade check2");
            isShowPosWidget = false;
            ui->myStackedWidget->setCurrentWidget(scanPosTradeTalk);
            //启动定时器
            scanPosTradeTalk->myTimer->start();
        }
        else
        {
            //重置定时器
            scanPosTradeTalk->myTimer->stop();
            scanPosTradeTalk->myTimer->start();
        }
    }
    else
        scanPosStmTalk->stmVoiceCmd(10);
}

void myPosWidget::processWeixinBeatAck_v1()
{
    weixinBeatAck_t weixinBeatAck;
    //scanPosGprsTalk->readweixinBeatAckInfo(&weixinBeatAck);
    scanPosNetTalk->readweixinBeatAckInfo(&weixinBeatAck);

    QString currentDateTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    unsigned char dateTime[14];
    for(int i=0; i<7; i++)
    {
        //qDebug("kira --- weixinBeatAck.time[%d]: %x", i, weixinBeatAck.time[i]);
        unsigned char dateTimeL = weixinBeatAck.time[i] & 0x0f;
        unsigned char dateTimeH = (weixinBeatAck.time[i]>>4) & 0x0f;
        dateTime[2*i] = HexToAsc(dateTimeH);
        dateTime[2*i+1] = HexToAsc(dateTimeL);
        //dateTime[2*i] = dateTimeH;
        //dateTime[2*i+1] = dateTimeL;
    }
    QString serverDateTime = QByteArray::fromRawData((char*)dateTime, sizeof(dateTime));
    qDebug("kira --- check time value!");
    qDebug(qPrintable(currentDateTime));
    qDebug(qPrintable(serverDateTime));

    double dateTimeDiff1 = currentDateTime.toDouble() - serverDateTime.toDouble();
    double dateTimeDiff2 = serverDateTime.toDouble() - currentDateTime.toDouble();
    if((dateTimeDiff1 > MAX_BEATTIMEDIFF) || (dateTimeDiff2 > MAX_BEATTIMEDIFF))
    {
        qDebug("kira --- use weixin server datetime check system datetime...");
        char cmdData[60] = {0};
        sprintf(cmdData, "date %.2x%.2x.%.2x.%.2x-%.2x:%.2x:%.2x",
                weixinBeatAck.time[0], weixinBeatAck.time[1], weixinBeatAck.time[2],
                weixinBeatAck.time[3], weixinBeatAck.time[4], weixinBeatAck.time[5],
                weixinBeatAck.time[6]);
        system(cmdData);
        system("hwclock -w");
    }

    QString keyVerStr = QByteArray::fromRawData((char*)weixinBeatAck.keyVer, sizeof(weixinBeatAck.keyVer));
    qDebug("kira --- compare key version: ");
    qDebug(qPrintable(keylistVer));
    qDebug(qPrintable(keyVerStr));
    if(keyVerStr != keylistVer)
    {

        configSet.writeSetInfo(configFile, Aliapygroup, "keylistVerTemp", keyVerStr);
        if(isAskKey && isAskBlack)
        {
            qDebug("kira --- different key version...");
            //isAskKey = false;
            tradeScanTimer->stop();
            askAlipayKeylist();
            myTimer->start();
        }
        return;
    }
    else
    {
        qDebug("kira --- same key version...");
    }

    QString blackVerStr = QByteArray::fromRawData((char*)weixinBeatAck.blackVer, sizeof(weixinBeatAck.blackVer));
    qDebug(qPrintable(blacklistVer));
    qDebug(qPrintable(blackVerStr));
    if(blackVerStr == "00000000")
    {
        qDebug("kira --- clear black info version = 00000000...");
        if(false == isClearBlankBlack)
        {
            isClearBlankBlack = true;
            scanPosDB->clearAlipayBlack();
            blacklistVer = "00000000";
            configSet.writeSetInfo(configFile, Aliapygroup, "blacklistVer", blacklistVer);
            configSet.writeSetInfo(configFile, Aliapygroup, "blackCurrentNum", "0");
        }
    }
    else if((blackVerStr != blacklistVer) && isAskBlack)
    {
        isClearBlankBlack = true;
        configSet.writeSetInfo(configFile, Aliapygroup, "blacklistVerTemp", blackVerStr);
        qDebug("kira --- different black version...");
        //isAskBlack = false;
        tradeScanTimer->stop();
        askAlipayBlacklist();
        myTimer->start();
    }
    else
    {
       qDebug("kira --- same black version...");
    }

    QString l_LVersion = QByteArray::fromRawData((char*)weixinBeatAck.sysVersion, sizeof(weixinBeatAck.sysVersion));
    QString l_SVersion = QByteArray::fromRawData((char*)weixinBeatAck.stVersion, sizeof(weixinBeatAck.stVersion));

    qDebug("kira --- LVersion: ");
    qDebug(qPrintable(l_LVersion));
    qDebug("kira--- SVersion: ");
    qDebug(qPrintable(l_SVersion));

    if(false == judgeFtpWork)
    {
        configSet.readSetInfo(configFile, workgroup, "LVersion", LVersion); // kira added - 2017.3.8
        configSet.readSetInfo(configFile, workgroup, "SVersion", SVersion);

        if(LVersion != l_LVersion)
        {
            LTempVersion = l_LVersion;
            askPosLVersionUpdata();
            judgeFtpWork = true;
            configSet.writeSetInfo(configFile, workgroup, "LTempVersion", l_LVersion);
        }
        else
        {
            qDebug("kira --- same LVersion");
            if(SVersion != l_SVersion)
            {
                STempVersion = l_SVersion;
                askPosSVersionUpdata();
                judgeFtpWork = true;
                sendUpdataInfo(1);
                configSet.writeSetInfo(configFile, workgroup, "STempVersion", l_SVersion);
                SVersion = l_SVersion;
                configSet.writeSetInfo(configFile, workgroup, "SVersion", l_SVersion);
            }
            else
            {
                qDebug("kira --- same SVersion");
            }
        }
    }

    if(true == judgeUp)
    {
        sendUpdataInfo(0);
    }
}

// v2 --- 2017.3.14
int myPosWidget::sendUpdataInfo(int type)
{
    int len = sizeof(updataInfo_t);
    updataInfo_t updataInfo;
    unsigned char upData[7+sizeof(updataInfo_t)] = {0};
    memset(upData, 0, sizeof(upData));
    memset(&updataInfo, 0, sizeof(updataInfo_t));
    upData[0] = 0x55;
    upData[1] = 0x7a;
    upData[2] = 0x35;
    memcpy(&upData[3], &len, 2);
    upData[5] = 0x00;
    memcpy(updataInfo.posID, posID.toLatin1().data(), posID.length());
    memcpy(updataInfo.cityID, cityID.toLatin1().data(), cityID.length());
    memcpy(updataInfo.busID, busID.toLatin1().data(), busID.length());
    memcpy(updataInfo.buslineID, buslineID.toLatin1().data(), buslineID.length());
    if(0 == type)
    {
        memcpy(updataInfo.oldVersion, LVersion.toLatin1().data(), LVersion.length());
        memcpy(updataInfo.newVersion, LTempVersion.toLatin1().data(), LTempVersion.length());
    }
    else
    {
        memcpy(updataInfo.oldVersion, SVersion.toLatin1().data(), SVersion.length());
        memcpy(updataInfo.newVersion, STempVersion.toLatin1().data(), STempVersion.length());
    }
    QString myDateTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    memcpy(updataInfo.time, myDateTime.toLatin1().data(), myDateTime.length());
    memcpy(&upData[6], &updataInfo, len);
    unsigned char check = 0;
    for(int i=0; i<len+6; i++)
        check ^= upData[i];
    upData[len+6] = check;

#if 1
    printf("kira --- updataInfo: ");
    for(int i=0; i<len+7; i++)
        printf("%.2x", upData[i]);
    printf("\n");
#endif

    int sendlen;
    scanPosNetTalk->socketSend(upData, sizeof(upData), &sendlen);
}

void myPosWidget::processUpdataInfoAck()
{
    updataInfpAck_t ackInfo;
    scanPosNetTalk->readUpdataAckInfo(&ackInfo);
    QString retStr = QByteArray::fromRawData(ackInfo.version, sizeof(ackInfo.version));
    printf("kira --- ackInfo.ret: %d \n", ackInfo.ret);
    qDebug("kira --- ackInfo.version: ");
    qDebug(qPrintable(retStr));
    if((LTempVersion == retStr) && ('0' == ackInfo.ret))
    {
        system("rm -rf /opt/kira/updata/newUpdate.notify");
        qDebug("kira --- pos updata info send success...");
        judgeUp = false;
        configSet.writeSetInfo(configFile, workgroup, "LVersion", LTempVersion);
    }
    else if((STempVersion == retStr) && ('0' == ackInfo.ret))
    {
        qDebug("kira --- pos updata info send success...");

    }
    else
    {
#if 0
        if(LTempVersion == retStr)
           sendUpdataInfo(0);
       else if(STempVersion == retStr)
           sendUpdataInfo(1);
#endif
       qDebug("kira --- updata info send failed send again...");
    }
}

// v2 --- 2017.3.15
void myPosWidget::processPscamAck(char retAck)
{
    if((unsigned char)retAck != 0x00){
        printf("qwy---psam check error\n");
        return;
    }
    // 添加数据库保存交易记录信息
    bool insertRet = scanPosDB->insertAlipayTradeInfoV2(vCityCardInfo, cityCardQR);
    if(false == insertRet)
    {
        printf("交易数据存储失败，请重刷。。。\n");
        //scanPosStmTalk->stmVoiceCmd(10);
        return;
    }
    //goto tradedialog
    scanPosStmTalk->setBalance(balance);
    scanPosStmTalk->setDebit(debit);
    //scanPosStmTalk->stmVoiceCmd(12);
    //scanPosStmTalk->stmVoiceCmd(26);

    scanPosTradeTalk->setBalance(balance);
    scanPosTradeTalk->setDebit(debit);

    scanPosScanTalk->tradeGledCtrl(1);
    if(isShowPosWidget)
    {
        isShowPosWidget = false;
        ui->myStackedWidget->setCurrentWidget(scanPosTradeTalk);
        //启动定时器
        scanPosTradeTalk->myTimer->start();
    }
    else
    {
        //重置定时器
        scanPosTradeTalk->myTimer->stop();
        scanPosTradeTalk->myTimer->start();
    }
    /*********************************************************************/
    int recordLen;
    int userIDLen;
    int cardNoLen;
    int cardDataLen;
    int cardTypeLen;
    memcpy(&recordLen, vCityCardInfo.offlineHeadInfo.offLineRecordLen, 4);
    memcpy(&userIDLen, vCityCardInfo.offlineHeadInfo.userIDLen, 4);
    memcpy(&cardNoLen, vCityCardInfo.offlineHeadInfo.cardNoLen, 4);
    memcpy(&cardDataLen, vCityCardInfo.offlineHeadInfo.cardDataLen, 4);
    memcpy(&cardTypeLen, vCityCardInfo.offlineHeadInfo.cardTypeLen, 4);
    // 组包
    int datalen = sizeof(offlineHeadInfoV2_t) + userIDLen
                   + recordLen + cardNoLen
                   + cardDataLen + cardTypeLen;

    //unsigned char tradeData[7+datalen] = {0};
    unsigned char tradeData[4096] = {0};
    memset(tradeData, 0, sizeof(tradeData));
    tradeData[0] = 0x55;
    tradeData[1] = 0x7a;
    tradeData[2] = 0x38;
    memcpy(&tradeData[3], &datalen, 2);
    tradeData[5] = 0x00;
    memcpy(&tradeData[6], &vCityCardInfo.offlineHeadInfo, sizeof(offlineHeadInfoV2_t));
    memcpy(&tradeData[6+sizeof(offlineHeadInfoV2_t)],
            vCityCardInfo.userID,
            userIDLen);
    memcpy(&tradeData[6+sizeof(offlineHeadInfoV2_t)+userIDLen],
            vCityCardInfo.record,
            recordLen);
    memcpy(&tradeData[6+sizeof(offlineHeadInfoV2_t)+userIDLen+recordLen],
            vCityCardInfo.cardNo,
            cardNoLen);
    memcpy(&tradeData[6+sizeof(offlineHeadInfoV2_t)+userIDLen+recordLen+cardNoLen],
            vCityCardInfo.cardData,
            cardDataLen);
    memcpy(&tradeData[6+sizeof(offlineHeadInfoV2_t)+userIDLen+recordLen+cardNoLen+cardDataLen],
            vCityCardInfo.cardType,
            cardTypeLen);
    qDebug("kira --- check ready send vcity information...");
    unsigned char check = 0;
    for(int i=0; i<datalen+6; i++)
        check ^= tradeData[i];
    tradeData[datalen+6] = check;

#if 1
    printf("kira --- tradeDataInfo: \n");
    for(int x=0; x<datalen+7; x++)
        printf("%.2x ", tradeData[x]);
    printf("\n");
#endif

    int sendlen;
    int ret = scanPosNetTalk->socketSend(tradeData, 7+datalen, &sendlen);
    if(1 == ret)
    {
        //scanPosStmTalk->stmVoiceCmd(10);
        qDebug("kira --- vcity发送交易记录失败...");
    }
}

// kira added - 2017.3.7
void myPosWidget::askPosLVersionUpdata()
{
    int len = sizeof(posVersionAskInfo_t);
    posVersionAskInfo_t posVersionAskInfo;
    unsigned char verData[7+sizeof(posVersionAskInfo_t)] = {0};
    memset(verData, 0, sizeof(verData));
    memset(&posVersionAskInfo, 0, sizeof(posVersionAskInfo_t));
    verData[0] = 0x55;
    verData[1] = 0x7a;
    verData[2] = 0x28;
    memcpy(&verData[3], &len, 2);
    verData[5] = 0x00;
    memcpy(posVersionAskInfo.posID, posID.toLatin1().data(), posID.length());
    memcpy(posVersionAskInfo.cityID, cityID.toLatin1().data(), cityID.length());
    memcpy(posVersionAskInfo.version, LVersion.toLatin1().data(), LVersion.length());
    memcpy(&verData[6], &posVersionAskInfo, len);
    unsigned char check = 0;
    for(int i=0; i<len+6; i++)
        check ^= verData[i];
    verData[len+6] = check;

#if 1
    printf("kira --- askPosLVersionUpdata: ");
    for(int i=0; i<len+7; i++)
        printf("%.2x", verData[i]);
    printf("\n");
#endif

    int sendlen;
    scanPosNetTalk->socketSend(verData, sizeof(verData), &sendlen);
}

void myPosWidget::askPosSVersionUpdata()
{
    int len = sizeof(posVersionAskInfo_t);
    posVersionAskInfo_t posVersionAskInfo;
    unsigned char verData[7+sizeof(posVersionAskInfo_t)] = {0};
    memset(verData, 0, sizeof(verData));
    memset(&posVersionAskInfo, 0, sizeof(posVersionAskInfo_t));
    verData[0] = 0x55;
    verData[1] = 0x7a;
    verData[2] = 0x28;
    memcpy(&verData[3], &len, 2);
    verData[5] = 0x00;
    memcpy(posVersionAskInfo.posID, posID.toLatin1().data(), posID.length());
    memcpy(posVersionAskInfo.cityID, cityID.toLatin1().data(), cityID.length());
    memcpy(posVersionAskInfo.version, SVersion.toLatin1().data(), SVersion.length());
    memcpy(&verData[6], &posVersionAskInfo, len);
    unsigned char check = 0;
    for(int i=0; i<len+6; i++)
        check ^= verData[i];
    verData[len+6] = check;

#if 0
    printf("kira --- askPosSVersionUpdata: ");
    for(int i=0; i<len+7; i++)
        printf("%.2x", verData[i]);
    printf("\n");
#endif

    int sendlen;
    scanPosNetTalk->socketSend(verData, sizeof(verData), &sendlen);
}

// Alipay
void myPosWidget::askAlipayKeylist()
{
    int len = sizeof(askAlipayKeyInfo_t);
    askAlipayKeyInfo_t askAlipayKeyInfo;
    unsigned char keyData[7+sizeof(askAlipayKeyInfo_t)] = {0};
    memset(keyData, 0, sizeof(keyData));
    memset(&askAlipayKeyInfo, 0, sizeof(askAlipayKeyInfo_t));
    keyData[0] = 0x55;
    keyData[1] = 0x7a;
    keyData[2] = 0x23;
    memcpy(&keyData[3], &len, 2);
    keyData[5] = 0x00;
    //QByteArray id = posID.toLatin1();
    //char *temp = id.data();
    //memcpy(askAlipayKeyInfo.deviceID, (unsigned char*)temp, sizeof(askAlipayKeyInfo.deviceID));
    memcpy(askAlipayKeyInfo.deviceID, (unsigned char*)posID.toLatin1().data(), posID.length());
    configSet.readSetInfo(configFile, Aliapygroup, "keyCurrentNum", keyCurrentNum);
    askAlipayKeyInfo.pageNum = keyCurrentNum.toInt();
    memcpy(&keyData[6], &askAlipayKeyInfo, len);
    unsigned char check = 0;
    for(int i=0; i<len+6; i++)
        check ^= keyData[i];
    keyData[len+6] = check;
    //scanPosGprsTalk->sendGprsData((char*)keyData, sizeof(keyData));
    int sendlen;
    scanPosNetTalk->socketSend(keyData, sizeof(keyData), &sendlen);
}

void myPosWidget::askAlipayBlacklist()
{
    int len = sizeof(askAlipayBlackInfo_t);
    askAlipayBlackInfo_t askAlipayBlackInfo;
    unsigned char blackData[7+sizeof(askAlipayBlackInfo_t)] = {0};
    memset(blackData, 0, sizeof(blackData));
    memset(&askAlipayBlackInfo, 0, sizeof(askAlipayBlackInfo_t));
    blackData[0] = 0x55;
    blackData[1] = 0x7a;
    blackData[2] = 0x25;
    memcpy(&blackData[3], &len, 2);
    blackData[5] = 0x00;
    //QByteArray id = posID.toLatin1();
    //char *temp = id.data();
    //memcpy(askAlipayBlackInfo.deviceID, (unsigned char*)temp, sizeof(askAlipayBlackInfo.deviceID));
    memcpy(askAlipayBlackInfo.deviceID, (unsigned char*)posID.toLatin1().data(), posID.length());
    configSet.readSetInfo(configFile, Aliapygroup, "blackCurrentNum", blackCurrentNum);
    int iblackCurrentNum = blackCurrentNum.toInt();
    memcpy(askAlipayBlackInfo.pageNum, (unsigned char*)&iblackCurrentNum, sizeof(askAlipayBlackInfo.pageNum));
    memcpy(&blackData[6], &askAlipayBlackInfo, len);
    unsigned char check = 0;
    for(int i=0; i<len+6; i++)
        check ^= blackData[i];
    blackData[len+6] = check;
    //scanPosGprsTalk->sendGprsData((char*)blackData, sizeof(blackData));
    int sendlen;
    scanPosNetTalk->socketSend(blackData, sizeof(blackData), &sendlen);
}

int myPosWidget::processAlipayScanInfo()
{
    int ret = 0, key_id = 0;
    char master_pub_key[128] = {0};
    char pos_param[256] = {0};

    alipayScanInfo_t alipayScanInfo;
    alipayTradeInfo_t alipayTradeInfo;
    memset(&alipayScanInfo, 0, sizeof(alipayScanInfo_t));
    memset(&alipayTradeInfo, 0, sizeof(alipayTradeInfo_t));
    scanPosScanTalk->getAlipayScanInfo(&alipayScanInfo);
    char ascScanData[MAX_ALIPAY_DATALEN*2] = {0};
    for(int n=0; n<alipayScanInfo.scanLen; n++)
    {
        ascScanData[2*n] = (char)HexToAsc((unsigned char)(alipayScanInfo.scanData[n]>>4)&0x0f);
        ascScanData[2*n+1] = (char)HexToAsc((unsigned char)alipayScanInfo.scanData[n]&0x0f);
    }
    QString QRcodeStr = QByteArray::fromRawData((char*)ascScanData, alipayScanInfo.scanLen*2);
    qDebug("kira --- QRcode: ");
    qDebug(qPrintable(QRcodeStr));

    qDebug("kira --- check get keyID...");
    QString myDateTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    /**
     * 调用接口获取密钥id
     */
    key_id = get_key_id((const unsigned char *)alipayScanInfo.scanData);
    //key_id = get_key_id(qrcode);
    if(key_id < 0)
    {
        qDebug("kira --- get keyID failed...");
        switch(key_id)
        {
            case MALFORMED_QRCODE:
                printf("获取密钥id失败，二维码错误!\n");
                //scanPosStmTalk->stmVoiceCmd(10);    // 请重刷
                break;
            default:
                break;
        }
        scanPosDB->insertAlipayAbnormalQR(QRcodeStr, myDateTime, key_id, 0);
        return 2;
    }
    qDebug("kira --- get keyID success key_id: %d...", key_id);
    //通过公钥id获取公钥 此处请商户自行实现
    //get_key_from_key_id(key_id, master_pub_key);
    QMap<int,QString>::iterator it = mykeyMap.find(key_id);
    if(it != mykeyMap.end())
    {
        memcpy(master_pub_key, it.value().toLatin1().data(), it.value().length());
        printf("kira --- get_key_from_key_id: \n");
        printf("%s \n", master_pub_key);
    }
    else
    {
        qDebug("kira --- no key info in POS...");
        return 2;
    }

    // cityID
    memcpy(alipayTradeInfo.offlineHeadInfo.cityID, cityID.toLatin1().data(), cityID.length());

    // POSID
    memcpy(alipayTradeInfo.offlineHeadInfo.posID, posID.toLatin1().data(), posID.length());

    // busID
    memcpy(alipayTradeInfo.offlineHeadInfo.busID, busID.toLatin1().data(), busID.length());

    // buslineID
    memcpy(alipayTradeInfo.offlineHeadInfo.busline, buslineID.toLatin1().data(), buslineID.length());

    // driverID
    //configSet.readSetInfo(configFile, workgroup, "driverID", driverID);
    qDebug(qPrintable(driverID));
    memcpy(alipayTradeInfo.offlineHeadInfo.driverID, driverID.toLatin1().data(), driverID.length());

    // ticketprice
    int amount = (int)(tickets.toDouble() * 100.0);
    memcpy(alipayTradeInfo.offlineHeadInfo.ticketPrice, &amount, 4);
    QString amountInteger = QString::number(amount/100, 10);
    int amountInt = amount%100;
    QString amountDecimalH = QString::number(amountInt/10, 10);
    QString amountDecimalL = QString::number(amountInt%10, 10);
    debit.clear();
    debit.append(amountInteger);
    debit.append(".");
    debit.append(amountDecimalH);
    debit.append(amountDecimalL);

    int intBalance = 0;
    QString balanceInteger = QString::number(intBalance/100, 10);
    int balanceInt = intBalance%100;
    QString balanceDecimalH = QString::number(balanceInt/10, 10);
    QString balanceDecimalL = QString::number(balanceInt%10, 10);
    balance.clear();
    balance.append(balanceInteger);
    balance.append(".");
    balance.append(balanceDecimalH);
    balance.append(balanceDecimalL);

    // location fill test data
    memcpy(alipayTradeInfo.offlineHeadInfo.longitude, "120.123E", sizeof(alipayTradeInfo.offlineHeadInfo.longitude));
    memcpy(alipayTradeInfo.offlineHeadInfo.latitude, "30.3452N", sizeof(alipayTradeInfo.offlineHeadInfo.latitude));

    // 填充时间
    QString currentDateTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    memcpy(alipayTradeInfo.offlineHeadInfo.tradeTime, currentDateTime.toLatin1().data(), currentDateTime.length());

    //填充终端参数
    QString param;
    QString tradeOrder;
    tradeOrder = QString("%1_%2_%3_%4").arg(posID).arg(buslineID).arg(currentDateTime).arg(tradeRecord);
    param = QString("{\"pos_id\":\"%1\",\"type\":\"SINGLE\",\"subject\":\"%2\",\"record_id\":\"%3\"}")
                .arg(posID).arg(buslineID).arg(tradeOrder);
    qDebug("kira --- param:");
    qDebug(qPrintable(param));
    qDebug("kira --- tradeOrder:");
    qDebug(qPrintable(tradeOrder));
    //pos_param = param.toLatin1().data();
    memcpy(pos_param, param.toLatin1().data(), param.length());
    for(int m=0; m<param.length(); m++)
        printf("%c", pos_param[m]);
    printf("\n");
    //pos_param = "{\"pos_id\":\"sh001\",\"type\":\"SINGLE\",\"subject\":\"bus192\",\"record_id\":\"sh001_20160514140218_000001\"}";
    // 填充tradeRecord
    memcpy(alipayTradeInfo.offlineHeadInfo.tradeRecord, &tradeRecord, 4);

    if(tradeRecord++ > 9000)
        tradeRecord = 0;

    //拼装验证请求
    VERIFY_REQUEST verify_request;
    //装入二进制格式的二维码
    verify_request.qrcode = (const unsigned char *)alipayScanInfo.scanData;
    //装入二进制二维码长度
    verify_request.qrcode_len = alipayScanInfo.scanLen;
    //装入公钥
    verify_request.master_pub_key = master_pub_key;
    //装入pos_param
    verify_request.pos_param = pos_param;
    //装入本次消费金额
    verify_request.amount_cent = amount;
    printf("kira --- amount: %d \n", verify_request.amount_cent);

    VERIFY_RESPONSE verify_response;
    verify_response.uid = (char*)malloc(17);
    verify_response.uid_len = 17;
    verify_response.record = (char*)malloc(4096);
    verify_response.record_len = 4096;

    /**
     * 调用接口验证二维码的有效性
     */
    //typedef int (*FunVerify)(VERIFY_REQUEST*, VERIFY_RESPONSE*);
    //FunVerify verify_qrcode = (FunVerify)posoffline_lib->resolve("verify_qrcode");

    //typedef int (*FunVerify)(VERIFY_REQUEST*, VERIFY_RESPONSE*);
    //FunVerify verify_qrcode = (FunVerify)dlsym(dp, "verify_qrcode");
    ret = verify_qrcode(&verify_request, &verify_response);

    /**
     * 处理返回的结果
     */
    if(ret != 1)
    {
        switch(ret)
        {
            case MALFORMED_QRCODE:
                printf("二维码格式错误！请提示用户二维码错误。\n");
                break;
            case QRCODE_INFO_EXPIRED:
                printf("二维码过期！请提示用户刷新二维码。\n");
                break;
            case QRCODE_KEY_EXPIRED:
                printf("二维码密钥过期！请提示用户联网后刷新二维码再使用。\n");
                break;
            case POS_PARAM_ERROR:
                printf("商户传入的pos_param错误，请检查传入的pos_param。\n");
                break;
            case QUOTA_EXCEEDED:
                printf("单笔额度超限！请提示用户由于额度限制无法过闸机。\n");
                break;
            case NO_ENOUGH_MEMORY:
                printf("内存不足，极端错误，请检查程序运行空间是否足够。\n");
                break;
            case SYSTEM_ERROR:
                printf("系统异常！请联系支付宝技术人员。\n");
                break;
            default:
                break;
        }
        printf("二维码校验结束！验证失败，不放行！\n");
        //scanPosStmTalk->stmVoiceCmd(10);
        free(verify_response.uid);
        free(verify_response.record);
        scanPosDB->insertAlipayAbnormalQR(QRcodeStr, myDateTime, key_id, ret);
        return 1;
    }

    // 填充userID, record
    qDebug("kira --- check get userID, record...");
    qDebug("kira --- verify_response.uid_len: %d...", verify_response.uid_len);
    qDebug("kira --- verify_response.record_len: %d...", verify_response.record_len);
    memcpy(alipayTradeInfo.offlineHeadInfo.userIDLen, &verify_response.uid_len, 4);
    memcpy(alipayTradeInfo.offlineHeadInfo.offLineRecordLen, &verify_response.record_len, 4);
    //alipayTradeInfo.userID = (char*)malloc(verify_response.uid_len);
    //alipayTradeInfo.record = (char*)malloc(verify_response.record_len);
    memcpy(alipayTradeInfo.userID, verify_response.uid, verify_response.uid_len);
    memcpy(alipayTradeInfo.record, verify_response.record, verify_response.record_len);

    QString userIDStr = QByteArray::fromRawData((char*)verify_response.uid, verify_response.uid_len);
    qDebug("kira --- userIDStr: ");
    qDebug(qPrintable(userIDStr));

    if(scanPosDB->findAlipayBlack(userIDStr))
    {
        qDebug("black QR... ");
        //scanPosStmTalk->stmVoiceCmd(10);
        free(verify_response.uid);
        free(verify_response.record);
        return 5;
    }

    // 添加同一个用户3S内不能重复扫码10分钟内一码一人交易
    int repeatRet = scanPosDB->judgeAlipayRepeatScan(alipayTradeInfo, QRcodeStr);
    if(2 == repeatRet)
    {
        qDebug("same QR repeat scan...");
        //scanPosStmTalk->stmVoiceCmd(10);
        free(verify_response.uid);
        free(verify_response.record);
        return 6;
    }
    else if(1 == repeatRet)
    {
        qDebug("please wait...");
        //scanPosStmTalk->stmVoiceCmd(10);
        free(verify_response.uid);
        free(verify_response.record);
        return 3;
    }
    // 添加数据库保存交易记录信息
    bool insertRet = scanPosDB->insertAlipayTradeInfo(alipayTradeInfo, QRcodeStr);
    if(false == insertRet)
    {
        printf("交易数据存储失败，请重刷。。。\n");
        //scanPosStmTalk->stmVoiceCmd(10);
        free(verify_response.uid);
        free(verify_response.record);
        return 4;
    }

    // 组包
    int datalen = sizeof(offlineHeadInfo_t) + verify_response.uid_len
                   + verify_response.record_len;

    //unsigned char tradeData[7+datalen] = {0};
    unsigned char tradeData[4096] = {0};
    memset(tradeData, 0, sizeof(tradeData));
    tradeData[0] = 0x55;
    tradeData[1] = 0x7a;
    tradeData[2] = 0x24;
    memcpy(&tradeData[3], &datalen, 2);
    tradeData[5] = 0x00;
    memcpy(&tradeData[6], &alipayTradeInfo.offlineHeadInfo, sizeof(offlineHeadInfo_t));
    memcpy(&tradeData[6+sizeof(offlineHeadInfo_t)],
            alipayTradeInfo.userID,
            verify_response.uid_len);
    memcpy(&tradeData[6+sizeof(offlineHeadInfo_t)+verify_response.uid_len],
            alipayTradeInfo.record,
            verify_response.record_len);
    qDebug("kira --- check ready send information...");
    unsigned char check = 0;
    for(int i=0; i<datalen+6; i++)
        check ^= tradeData[i];
    tradeData[datalen+6] = check;

#if 0
    printf("kira --- tradeDataInfo: \n");
    for(int x=0; x<datalen+7; x++)
        printf("%.2x ", tradeData[x]);
    printf("\n");
#endif

    //ret = scanPosGprsTalk->sendGprsData((char*)tradeData, sizeof(tradeData));
    //ret = scanPosGprsTalk->sendGprsData((char*)tradeData, 7+datalen);
    int sendlen;
    ret = scanPosNetTalk->socketSend(tradeData, 7+datalen, &sendlen);
    if(1 == ret)
    {
        //scanPosStmTalk->stmVoiceCmd(10);
        qDebug("kira --- 发送交易记录失败...");
    }

    free(verify_response.uid);
    free(verify_response.record);
    //free(alipayTradeInfo.userID);
    //free(alipayTradeInfo.record);
    return 0;
}

// v2 --- 2017.3.9
#define CARD_TYPE_LIST "[\"T0330100\",\"T1330100\",\"ANT00001\"]"
#define KEY_LIST "[{\"key_id\":0,\"public_key\":\"028B90D7C281AD21B4BA9492C522E39B655890CC41420EB8E8826B28C6DF2D467A\"},\
                    {\"key_id\":1,\"public_key\":\"03F04C74CAD7EC0F2AE93AC51EABCE95D3D3E6F32F71816D7D838308578425F873\"},\
                    {\"key_id\":2,\"public_key\":\"0307F46E2C7F40B7B89F11A85C11B0C9374C886132073FAE0BD54A940FAEB21907\"},\
                    {\"key_id\":3,\"public_key\":\"034532FF3BEBE61DAC18B7F0D7A201A5080D49F2EA9B256669728A4E8AC9D0DC76\"},\
                    {\"key_id\":4,\"public_key\":\"0362650BA9947153AB38D6C1F63D3AFFDF1CBD199A7E3EFDD4F61BC6F33F0F8F76\"},\
                    {\"key_id\":5,\"public_key\":\"0268A244029140F693BCA395E5BFA79D6B7B7F1E3D232AFDE2955BAE9A2DBB8E2B\"},\
                    {\"key_id\":6,\"public_key\":\"028B90D7C281AD21B4BA9492C522E39B655890CC41420EB8E8826B28C6DF2D467A\"},\
                    {\"key_id\":7,\"public_key\":\"03B31505F0622E381E3C53211CD44EB4A3FB35DA7094E5F09FA89D91DCC7165929\"},\
                    {\"key_id\":8,\"public_key\":\"021ADB0F0E05A7B9BE0C11153E4D0BC3859F5F73EAE9BEADF04160DE9E4BBF9C74\"},\
                    {\"key_id\":9,\"public_key\":\"0255668F6A4CCE5272A7FC77A29902D06E14D939CB50DDC071ACB86B54FB3FCD91\"},\
                    {\"key_id\":10,\"public_key\":\"02E740AB065F7976C836B32C324FEC665DF034EE2CBEB3C311C14EA59E4494F5AC\"},\
                    {\"key_id\":11,\"public_key\":\"02EF5B9310CC93ADC130D71F605474FDBEF284A1AE5AF7493A3A982AE1031BAAC2\"},\
                    {\"key_id\":12,\"public_key\":\"031E0F70A8A5460365F91A7D66620BD9424EAAEBD9CFF54E84CB4257C5B32324D9\"},\
                    {\"key_id\":13,\"public_key\":\"02D1BEB9BAC72681B9CECBE7EF369663A04D4487EE20CC10673E12920A01FD4189\"},\
                    {\"key_id\":14,\"public_key\":\"02393BF79609919DDDE78FBCD0579C4995B04FA84E74D9802D78BC025B777114B9\"},\
                    {\"key_id\":15,\"public_key\":\"03C2A0C0D0163CA194FA6F9DCF71C02154D1ECD9CEB77C297F32CF47AA2C9DF22A\"},\
                    {\"key_id\":16,\"public_key\":\"02F27F0CFF4D56D513F427D4274B862C4998B3F03FBC59275E7E0E37EA5E492311\"},\
                    {\"key_id\":17,\"public_key\":\"03ECADBF719F9C6539534F376A210486888A89B952E8A421A10D25E4601C667C74\"},\
                    {\"key_id\":18,\"public_key\":\"0296BD24657AB0E5D4B7A6A8A0D41E5E722CA3AD791B74C8EC2BBFDBD9D9A6F571\"},\
                    {\"key_id\":19,\"public_key\":\"0394657638D9FD20B463AF15F761E0F42E94346D35D8F494445C89AAE7A1FA6838\"},\
                    {\"key_id\":20,\"public_key\":\"0366050BC5FFA296753607EEC07B811C2A59FB93F2ED41D30D081797267B8CB191\"},\
                    {\"key_id\":21,\"public_key\":\"02878B02CF937EED619B85BAADAC24E408BF84E3C428F7AF4CA8D28628DDE2E36C\"},\
                    {\"key_id\":22,\"public_key\":\"02A2E791A0AC515A11AA2AEF7207A91AF9A74A1C22297F107DCDFB8CAB75E65CCA\"},\
                    {\"key_id\":23,\"public_key\":\"0375D631050A21B80C3B2443D93804067309D5AD7D22CE1D6771D540BDCD499521\"},\
                    {\"key_id\":24,\"public_key\":\"02CCA330425FEEF7B4070BC6F605D5703BE2444BAEE16415484CC0D905A2203B12\"},\
                    {\"key_id\":25,\"public_key\":\"02F48FB87407A799198350F83586595C5F10F1978DDD944360D16A8086FBC03368\"},\
                    {\"key_id\":26,\"public_key\":\"021964E5C29653C261136F43437C31D2C6A2AC1849CB79829731868E9D2947CE85\"},\
                    {\"key_id\":27,\"public_key\":\"0318B0F354FB34B0F024BDB689F647DB5BAADA2D91BCAA66DC514059908DAE3C6B\"},\
                    {\"key_id\":28,\"public_key\":\"023A391EE698ACB20AD40D42E3D2A546DC0D998D5BEFB626DEC874CDABADD9963F\"},\
                    {\"key_id\":29,\"public_key\":\"025D296816661C34CA0C1E528339B01434A98C6CB149BD99C6AA6F37B65E6086B0\"},\
                    {\"key_id\":30,\"public_key\":\"0200238F19D827135A2C194D2B52AD681FF1156196EE654C38BB49A9CB8EBA9F7C\"},\
                    {\"key_id\":31,\"public_key\":\"02EA31FB09DD181E6645D5E1827CDE07FC7D23BCD9DFB1C3DECDF6534F0B4713B9\"}]"
int myPosWidget::processAlipayScanInfoV2()
{
    int ret = 0;
    char pos_param[256] = {0};

    alipayScanInfo_t alipayScanInfo;
    alipayTradeInfoV2_t alipayTradeInfo;
    memset(&alipayScanInfo, 0, sizeof(alipayScanInfo_t));
    memset(&alipayTradeInfo, 0, sizeof(alipayTradeInfoV2_t));
    scanPosScanTalk->getAlipayScanInfo(&alipayScanInfo);
    char ascScanData[MAX_ALIPAY_DATALEN*2] = {0};
    for(int n=0; n<alipayScanInfo.scanLen; n++)
    {
        ascScanData[2*n] = (char)HexToAsc((unsigned char)(alipayScanInfo.scanData[n]>>4)&0x0f);
        ascScanData[2*n+1] = (char)HexToAsc((unsigned char)alipayScanInfo.scanData[n]&0x0f);
    }
    QString QRcodeStr = QByteArray::fromRawData((char*)ascScanData, alipayScanInfo.scanLen*2);
    qDebug("kira --- QRcode: ");
    qDebug(qPrintable(QRcodeStr));

    qDebug("kira --- check get keyID...");
    QString myDateTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");

    QMap<int, QString>::iterator it;
    QMap<int, QString>::iterator it2;
    QString keyStr;
    for(it=mykeyMap.begin(); it!=mykeyMap.end(); ++it)
    {
        QString str = QString("{\"key_id\":%1,\"public_key\":\"%2\"}").arg(it.key()).arg(it.value());
        keyStr.append(str);
        it2 = it;
        it2++;
        if(it2 != mykeyMap.end())
            keyStr.append(",");
    }
    QString keyStr2 = QString("[%1]").arg(keyStr);

    qDebug(qPrintable(keyStr2));
    QString cardTypeStr;;
    for(it=mycardTypeMap.begin(); it!=mycardTypeMap.end(); ++it)
    {
        QString str = QString("\"%1\"").arg(it.value());
        keyStr.append(str);
        it2 = it;
        it2++;
        if(it2 != mycardTypeMap.end())
            cardTypeStr.append(",");
    }
    QString cardTypeStr2 = QString("[%1]").arg(cardTypeStr);
    //qDebug(qPrintable(cardTypeStr2));

    const char* key_list = KEY_LIST;
    //qDebug("%s", key_list);
    //qDebug("%s", keyStr.toLatin1().data());
    const char* card_type_list = CARD_TYPE_LIST;

    //ret = init_pos_verify(keyStr2.toLatin1().data(), cardTypeStr2.toLatin1().data());
    //ret = init_pos_verify(key_list, card_type_list);
    ret = init_pos_verify(keyStr2.toLatin1().data(), card_type_list);
    if(ret != SUCCESS){
        printf("初始化POS失败！\n");
        switch(ret){
            case ILLEGAL_PARAM:
                printf("初始化参数格式错误！请检查参数是否符合json列表格式且各字段正确。\n");
            break;
            case NO_ENOUGH_MEMORY:
                printf("内存不足，极端错误，请检查程序运行空间是否足够。\n");
            break;
            case SYSTEM_ERROR:
                printf("系统异常！请联系支付宝技术人员。\n");
            break;
            default:
            break;
        }
        scanPosDB->insertAlipayAbnormalQR(QRcodeStr, myDateTime, 0, ret);
        return 2;
    }

    // cityID
    memcpy(alipayTradeInfo.offlineHeadInfo.cityID, cityID.toLatin1().data(), cityID.length());
    // POSID
    memcpy(alipayTradeInfo.offlineHeadInfo.posID, posID.toLatin1().data(), posID.length());
    // busID
    memcpy(alipayTradeInfo.offlineHeadInfo.busID, busID.toLatin1().data(), busID.length());
    // buslineID
    memcpy(alipayTradeInfo.offlineHeadInfo.busline, buslineID.toLatin1().data(), buslineID.length());
    // driverID
    //configSet.readSetInfo(configFile, workgroup, "driverID", driverID);
    qDebug(qPrintable(driverID));
    memcpy(alipayTradeInfo.offlineHeadInfo.driverID, driverID.toLatin1().data(), driverID.length());

    // ticketprice
    int amount = (int)(tickets.toDouble() * 100.0);
    memcpy(alipayTradeInfo.offlineHeadInfo.ticketPrice, &amount, 4);
    QString amountInteger = QString::number(amount/100, 10);
    int amountInt = amount%100;
    QString amountDecimalH = QString::number(amountInt/10, 10);
    QString amountDecimalL = QString::number(amountInt%10, 10);
    debit.clear();
    debit.append(amountInteger);
    debit.append(".");
    debit.append(amountDecimalH);
    debit.append(amountDecimalL);

    int intBalance = 0;
    QString balanceInteger = QString::number(intBalance/100, 10);
    int balanceInt = intBalance%100;
    QString balanceDecimalH = QString::number(balanceInt/10, 10);
    QString balanceDecimalL = QString::number(balanceInt%10, 10);
    balance.clear();
    balance.append(balanceInteger);
    balance.append(".");
    balance.append(balanceDecimalH);
    balance.append(balanceDecimalL);

    // location fill test data
    gpsSerialInfo gpsInfo;
    scanPosGpsTalk->getGpsInfo(&gpsInfo);
    QString longitudeValueStr = QByteArray::fromRawData((char*)gpsInfo.longitude, sizeof(gpsInfo.longitude));
    //QString longitudeSignStr = QByteArray::fromRawData((char*)gpsInfo.longitude_sign, 1);
    //QString longitudeStr = longitudeValueStr + longitudeSignStr;
    qDebug("kira --- longitudeStr:");
    qDebug(qPrintable(longitudeValueStr));
    QString latitudeValueStr = QByteArray::fromRawData((char*)gpsInfo.latitude, sizeof(gpsInfo.latitude));
    //QString latitudeSignStr = QByteArray::fromRawData((char*)gpsInfo.latitude_sign, 1);
    //QString latitudeStr = latitudeValueStr + latitudeSignStr;
    qDebug("kira --- latitudeStr:");
    qDebug(qPrintable(latitudeValueStr));
    memcpy(alipayTradeInfo.offlineHeadInfo.longitude, longitudeValueStr.toLatin1().data(), sizeof(alipayTradeInfo.offlineHeadInfo.longitude));
    memcpy(alipayTradeInfo.offlineHeadInfo.latitude, latitudeValueStr.toLatin1().data(), sizeof(alipayTradeInfo.offlineHeadInfo.latitude));
    //memcpy(alipayTradeInfo.offlineHeadInfo.longitude, "120.123E", sizeof(alipayTradeInfo.offlineHeadInfo.longitude));
    //memcpy(alipayTradeInfo.offlineHeadInfo.latitude, "30.3452N", sizeof(alipayTradeInfo.offlineHeadInfo.latitude));

    // 填充时间
    QString currentDateTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    memcpy(alipayTradeInfo.offlineHeadInfo.tradeTime, currentDateTime.toLatin1().data(), currentDateTime.length());

    //填充终端参数
    QString param;
    QString tradeOrder;
    tradeOrder = QString("%1_%2_%3_%4").arg(posID).arg(buslineID).arg(currentDateTime).arg(tradeRecord);
    param = QString("{\"pos_id\":\"%1\",\"type\":\"SINGLE\",\"subject\":\"%2\",\"record_id\":\"%3\"}")
                .arg(posID).arg(buslineID).arg(tradeOrder);
    qDebug("kira --- param:");
    qDebug(qPrintable(param));
    qDebug("kira --- tradeOrder:");
    qDebug(qPrintable(tradeOrder));
    //pos_param = param.toLatin1().data();
    memcpy(pos_param, param.toLatin1().data(), param.length());
    for(int m=0; m<param.length(); m++)
        printf("%c", pos_param[m]);
    printf("\n");
    //pos_param = "{\"pos_id\":\"sh001\",\"type\":\"SINGLE\",\"subject\":\"bus192\",\"record_id\":\"sh001_20160514140218_000001\"}";
    // 填充tradeRecord
    memcpy(alipayTradeInfo.offlineHeadInfo.tradeRecord, &tradeRecord, 4);

    if(tradeRecord++ > 9000)
        tradeRecord = 0;

    //拼装验证请求
    VERIFY_REQUEST_V2 verify_request;
    //装入二进制格式的二维码
    verify_request.qrcode = (const unsigned char *)alipayScanInfo.scanData;
    //装入二进制二维码长度
    verify_request.qrcode_len = alipayScanInfo.scanLen;
    //装入pos_param
    verify_request.pos_param = pos_param;
    //装入本次消费金额
    verify_request.amount_cent = amount;
    printf("kira --- amount: %d \n", verify_request.amount_cent);

    VERIFY_RESPONSE_V2 verify_response;
    verify_response.uid = (char*)malloc(17);
    verify_response.uid_len = 17;
    verify_response.record = (char*)malloc(2048);
    verify_response.record_len = 2048;
    verify_response.card_no = (char*)malloc(32);
    verify_response.card_no_len = 32;
    verify_response.card_data = (unsigned char*)malloc(128);
    verify_response.card_data_len = 128;
    verify_response.card_type = (char*)malloc(16);
    verify_response.card_type_len = 16;

    /**
     * 调用接口验证二维码的有效性
     */
    ret = verify_qrcode_v2(&verify_request, &verify_response);

    /**
     * 处理返回的结果
     */
    if(ret != SUCCESS)
    {
        int reterr = 1;
        switch(ret){
            case MALFORMED_QRCODE:
                printf("二维码格式错误！请提示用户二维码错误。\n");
            break;
            case QRCODE_INFO_EXPIRED:
                printf("二维码过期！请提示用户刷新二维码。\n");
            break;
            case QRCODE_KEY_EXPIRED:
                printf("二维码密钥过期！请提示用户联网后刷新二维码再使用。\n");
            break;
            case POS_PARAM_ERROR:
                printf("商户传入的pos_param错误，请检查传入的pos_param。\n");
            break;
            case QUOTA_EXCEEDED:
                printf("单笔额度超限！请提示用户由于额度限制无法过闸机。\n");
            break;
            case NO_ENOUGH_MEMORY:
                printf("内存不足，极端错误，请检查程序运行空间是否足够。\n");
            break;
            case QRCODE_DUPLICATED:
                printf("二维码重复！验证失败。\n");
                reterr = 7;
            break;
            case SYSTEM_ERROR:
                printf("系统异常！请联系支付宝技术人员。\n");
            break;
            default:
            break;
        }
        printf("二维码校验结束！验证失败，不放行！\n");
        printf("===========验证二维码例程 结束================\n");
        free(verify_response.uid);
        free(verify_response.record);
        free(verify_response.card_no);
        free(verify_response.card_data);
        free(verify_response.card_type);
        scanPosDB->insertAlipayAbnormalQR(QRcodeStr, myDateTime, 0, ret);
        return reterr;
    }
    printf("从二维码中获取到的uid: %s\n", verify_response.uid);
    printf("验证成功后，返还的脱机记录: %s\n", verify_response.record);
    printf("二维码中的卡类型为: %s\n",verify_response.card_type);
    printf("二维码中的卡号为: %s\n", verify_response.card_no);

    // 填充userID, record
    qDebug("kira --- check get userID, record...");
    qDebug("kira --- verify_response.uid_len: %d...", verify_response.uid_len);
    qDebug("kira --- verify_response.record_len: %d...", verify_response.record_len);
    qDebug("kira --- verify_response.card_no_len: %d...", verify_response.card_no_len);
    qDebug("kira --- verify_response.card_data_len: %d...", verify_response.card_data_len);
    qDebug("kira --- verify_response.card_type_len: %d...", verify_response.card_type_len);

    char l_cardData[256] = {0};
    int l_cardDataLen = verify_response.card_data_len*2;
    for(int m=0; m<l_cardDataLen; m++)
    {
        l_cardData[2*m] = (char)HexToAsc((unsigned char)(verify_response.card_data[m]>>4)&0x0f);
        l_cardData[2*m+1] = (char)HexToAsc((unsigned char)verify_response.card_data[m]&0x0f);
    }

    memcpy(alipayTradeInfo.offlineHeadInfo.userIDLen, &verify_response.uid_len, 4);
    memcpy(alipayTradeInfo.offlineHeadInfo.offLineRecordLen, &verify_response.record_len, 4);
    memcpy(alipayTradeInfo.offlineHeadInfo.cardNoLen, &verify_response.card_no_len, 4);
    memcpy(alipayTradeInfo.offlineHeadInfo.cardDataLen, &l_cardDataLen, 4);
    memcpy(alipayTradeInfo.offlineHeadInfo.cardTypeLen, &verify_response.card_type_len, 4);
    memcpy(alipayTradeInfo.userID, verify_response.uid, verify_response.uid_len);
    memcpy(alipayTradeInfo.record, verify_response.record, verify_response.record_len);
    memcpy(alipayTradeInfo.cardNo, verify_response.card_no, verify_response.card_no_len);
    memcpy(alipayTradeInfo.cardData, l_cardData, l_cardDataLen);
    memcpy(alipayTradeInfo.cardType, verify_response.card_type, verify_response.card_type_len);

    memcpy(&vCityCardInfo, &alipayTradeInfo, sizeof(alipayTradeInfoV2_t));
    cityCardQR = QRcodeStr;
    QString userIDStr = QByteArray::fromRawData((char*)verify_response.uid, verify_response.uid_len);
    qDebug("kira --- userIDStr: ");
    qDebug(qPrintable(userIDStr));

    if(scanPosDB->findAlipayBlack(userIDStr))
    {
        qDebug("black QR... ");
        //scanPosStmTalk->stmVoiceCmd(10);
        free(verify_response.uid);
        free(verify_response.record);
        free(verify_response.card_no);
        free(verify_response.card_data);
        free(verify_response.card_type);
        return 5;
    }

    // 添加同一个用户3S内不能重复扫码10分钟内一码一人交易
    int repeatRet = scanPosDB->judgeAlipayRepeatScanV2(alipayTradeInfo, QRcodeStr);
    if(2 == repeatRet)
    {
        qDebug("same QR repeat scan...");
        //scanPosStmTalk->stmVoiceCmd(10);
        free(verify_response.uid);
        free(verify_response.record);
        free(verify_response.card_no);
        free(verify_response.card_data);
        free(verify_response.card_type);
        return 6;
    }
    else if(1 == repeatRet)
    {
        qDebug("please wait...");
        //scanPosStmTalk->stmVoiceCmd(10);
        free(verify_response.uid);
        free(verify_response.record);
        free(verify_response.card_no);
        free(verify_response.card_data);
        free(verify_response.card_type);
        return 3;
    }

    QString cardType_str = QByteArray::fromRawData(verify_response.card_type, verify_response.card_type_len);
    if("ANT00001" == cardType_str)
    {
        // 添加数据库保存交易记录信息
        bool insertRet = scanPosDB->insertAlipayTradeInfoV2(alipayTradeInfo, QRcodeStr);
        if(false == insertRet)
        {
            printf("交易数据存储失败，请重刷。。。\n");
            //scanPosStmTalk->stmVoiceCmd(10);
            free(verify_response.uid);
            free(verify_response.record);
            free(verify_response.card_no);
            free(verify_response.card_data);
            free(verify_response.card_type);
            return 4;
        }

        // 组包
        int datalen = sizeof(offlineHeadInfoV2_t) + verify_response.uid_len
                       + verify_response.record_len + verify_response.card_no_len
                       + verify_response.card_data_len*2 + verify_response.card_type_len;

        //unsigned char tradeData[7+datalen] = {0};
        unsigned char tradeData[4096] = {0};
        memset(tradeData, 0, sizeof(tradeData));
        tradeData[0] = 0x55;
        tradeData[1] = 0x7a;
        tradeData[2] = 0x38;
        memcpy(&tradeData[3], &datalen, 2);
        tradeData[5] = 0x00;
        memcpy(&tradeData[6], &alipayTradeInfo.offlineHeadInfo, sizeof(offlineHeadInfoV2_t));
        memcpy(&tradeData[6+sizeof(offlineHeadInfoV2_t)],
                alipayTradeInfo.userID,
                verify_response.uid_len);
        memcpy(&tradeData[6+sizeof(offlineHeadInfoV2_t)+verify_response.uid_len],
                alipayTradeInfo.record,
                verify_response.record_len);
        memcpy(&tradeData[6+sizeof(offlineHeadInfoV2_t)+verify_response.uid_len+verify_response.record_len],
                alipayTradeInfo.cardNo,
                verify_response.card_no_len);
        memcpy(&tradeData[6+sizeof(offlineHeadInfoV2_t)+verify_response.uid_len+verify_response.record_len+verify_response.card_no_len],
                alipayTradeInfo.cardData,
                verify_response.card_data_len*2);
        memcpy(&tradeData[6+sizeof(offlineHeadInfoV2_t)+verify_response.uid_len+verify_response.record_len+verify_response.card_no_len+verify_response.card_data_len*2],
                alipayTradeInfo.cardType,
                verify_response.card_type_len);
        qDebug("kira --- check ready send information...");
        unsigned char check = 0;
        for(int i=0; i<datalen+6; i++)
            check ^= tradeData[i];
        tradeData[datalen+6] = check;

    #if 1
        printf("kira --- tradeDataInfo: \n");
        for(int x=0; x<datalen+7; x++)
            printf("%.2x ", tradeData[x]);
        printf("\n");
    #endif

        //ret = scanPosGprsTalk->sendGprsData((char*)tradeData, sizeof(tradeData));
        //ret = scanPosGprsTalk->sendGprsData((char*)tradeData, 7+datalen);
        int sendlen;
        ret = scanPosNetTalk->socketSend(tradeData, 7+datalen, &sendlen);
        if(1 == ret)
        {
            //scanPosStmTalk->stmVoiceCmd(10);
            qDebug("kira --- 发送交易记录失败...");
        }

        free(verify_response.uid);
        free(verify_response.record);
        free(verify_response.card_no);
        free(verify_response.card_data);
        free(verify_response.card_type);
        return 0;
    }
    else
    {
        // send city card psam info
        pscamInfo_t pscamInfo;
        pscamInfo.cardNoLen = verify_response.card_no_len;
        pscamInfo.cardDataLen = verify_response.card_data_len;
        memcpy(pscamInfo.cardNo, verify_response.card_no, verify_response.card_no_len);
        memcpy(pscamInfo.cardData, verify_response.card_data, verify_response.card_data_len);

        scanPosStmTalk->stmSendPsamInfo(pscamInfo);

        free(verify_response.uid);
        free(verify_response.record);
        free(verify_response.card_no);
        free(verify_response.card_data);
        free(verify_response.card_type);
        return 7;

    }


}

/**
 * 通过公钥id获取公钥
 */
void myPosWidget::get_key_from_key_id(int key_id, char* master_pub_key)
{
    QMap<int,QString>::iterator it = mykeyMap.find(key_id);
    master_pub_key = it.value().toLatin1().data();
    qDebug("kira --- keyValue: ");
    qDebug(qPrintable(it.value()));
    for(int i=0; i<it.value().length(); i++)
        printf("%c", master_pub_key[i]);
    printf("\n");
    printf("%s \n", master_pub_key);
}

int myPosWidget::initAlipayKey()
{
    bool keyret = scanPosDB->getAlipayKey(mykeyMap);
    if(keyret)
    {
        qDebug("kira --- Initial Alipay key success!");
#if 0
        QMap<int, QString>::iterator it; //遍历map
        for(it=mykeyMap.begin(); it!=mykeyMap.end(); ++it)
        {
            qDebug("%d: %s", it.key(), it.value().toLatin1().data());
        }
#endif
        return 0;
    }
    else
    {
        qDebug("kira --- Initial Alipay key failed!");
        return 1;
    }
}

int myPosWidget::initAlipayBlack()
{
    bool blackret = scanPosDB->getAlipayBlack(myblackMap);
    if(blackret)
    {
        qDebug("kira --- Initial Alipay black success!");
#if 0
        QMap<int, QString>::iterator it; //遍历map
        for(it=myblackMap.begin(); it!=myblackMap.end(); ++it)
        {
            qDebug("%d: %s", it.key(), it.value().toLatin1().data());
        }
#endif
        return 0;
    }
    else
    {
        qDebug("kira --- Initial Alipay black failed!");
        return 1;
    }
}

// v2 --- 2017.3.10
int myPosWidget::initAlipayCardType()
{
    bool cardTyperet = scanPosDB->getAlipayCardType(mycardTypeMap);
    if(cardTyperet)
    {
        qDebug("kira --- Initial Alipay cardType success!");
#if 0
        QMap<int, QString>::iterator it; //遍历map
        for(it=mycardTypeMap.begin(); it!=mycardTypeMap.end(); ++it)
        {
            qDebug("%d: %s", it.key(), it.value().toLatin1().data());
        }
#endif
        return 0;
    }
    else
    {
        qDebug("kira --- Initial Alipay cardType failed!");
        return 1;
    }
}

void myPosWidget::processAlipayBlackInfo_v1()
{
    myTimer->stop();
    alipayBlackInfo_t lalipayBlackInfo;
    //scanPosGprsTalk->readAlipayBlackInfo(&lalipayBlackInfo);
    scanPosNetTalk->readAlipayBlackInfo(&lalipayBlackInfo);
    qDebug("kira --- read AliapyBlackInfo check...");
    isAskBlack = false;
    for(int i=0; i<lalipayBlackInfo.num; i++)
    {
        QMap<int, QString>::iterator it = myblackMap.find(lalipayBlackInfo.black[i].blackOrder);
        QString valueStr = QByteArray::fromRawData((char*)lalipayBlackInfo.black[i].blackValue, sizeof(lalipayBlackInfo.black[i].blackValue));
        if(it != myblackMap.end())
        {
            if(valueStr != it.value())
            {
                myblackMap[lalipayBlackInfo.black[i].blackOrder] = it.value();
            }
            // updata
            //scanPosDB->updataAlipayBlack(lalipayBlackInfo.black[i].blackOrder, lalipayBlackInfo.black[i].blackValue);
            scanPosDB->updataAlipayBlack(lalipayBlackInfo.black[i].blackOrder, valueStr);
        }
        else
        {
            myblackMap.insert(lalipayBlackInfo.black[i].blackOrder, valueStr);
            // insert
            //scanPosDB->insertAlipayBlack(lalipayBlackInfo.black[i].blackOrder, lalipayBlackInfo.black[i].blackValue);
            scanPosDB->insertAlipayBlack(lalipayBlackInfo.black[i].blackOrder, valueStr);
        }
    }

#ifdef ALIPAYBLACK_DEBUG
    qDebug("kira ---  v1 Updata Alipay black success!");
    QMap<int, QString>::iterator blackInfo; //遍历map
    for(blackInfo=myblackMap.begin(); blackInfo!=myblackMap.end(); ++blackInfo)
    {
        qDebug("%d: %s", blackInfo.key(), blackInfo.value().toLatin1().data());
    }
#endif

    QString currentBlackStr = QString("%1").arg(lalipayBlackInfo.currentNum);
    //QString pagenumBlackStr = QString("%1").arg(lalipayBlackInfo.pageNum-1);
    QString pagenumBlackStr;
    if(lalipayBlackInfo.pageNum > 0)
        pagenumBlackStr = QString("%1").arg(lalipayBlackInfo.pageNum-1);
    else
        pagenumBlackStr = QString("%1").arg(lalipayBlackInfo.pageNum);
    qDebug(qPrintable(currentBlackStr));
    qDebug(qPrintable(pagenumBlackStr));

    if(currentBlackStr == pagenumBlackStr)
    {
        QString version;
        configSet.readSetInfo(configFile, Aliapygroup, "blacklistVerTemp", version);
        blacklistVer = version;
        configSet.writeSetInfo(configFile, Aliapygroup, "blacklistVer", blacklistVer);
        configSet.writeSetInfo(configFile, Aliapygroup, "blackCurrentNum", "0");
        isAskBlack = true;
    }
    else
    {
        QString ncurrentBlackStr = QString("%1").arg(lalipayBlackInfo.currentNum + 1);
        configSet.writeSetInfo(configFile, Aliapygroup, "blackCurrentNum", ncurrentBlackStr);
        askAlipayBlacklist();
        myTimer->start();
    }
}

void myPosWidget::processAlipayBlackInfo_v2()
{
    myTimer->stop();
    alipayBlackInfo_t lalipayBlackInfo;
    //scanPosGprsTalk->readAlipayBlackInfo(&lalipayBlackInfo);
    scanPosNetTalk->readAlipayBlackInfo(&lalipayBlackInfo);
    qDebug("kira --- read AliapyBlackInfo check...");
    if(isAskBlack)
    {
        scanPosDB->clearAlipayBlack();
        myblackMap.clear();
    }
    isAskBlack = false;

    for(int i=0; i<lalipayBlackInfo.num; i++)
    {
        QString valueStr = QByteArray::fromRawData((char*)lalipayBlackInfo.black[i].blackValue, sizeof(lalipayBlackInfo.black[i].blackValue));
        //myblackMap.insert(lalipayBlackInfo.black[i].blackOrder, lalipayBlackInfo.black[i].blackValue);
        myblackMap.insert(lalipayBlackInfo.black[i].blackOrder, valueStr);
        // insert
        //scanPosDB->insertAlipayBlack(lalipayBlackInfo.black[i].blackOrder, lalipayBlackInfo.black[i].blackValue);
        scanPosDB->insertAlipayBlack(lalipayBlackInfo.black[i].blackOrder, valueStr);
    }

    QString currentBlackStr = QString("%1").arg(lalipayBlackInfo.currentNum);
    QString pagenumBlackStr;
    if(lalipayBlackInfo.pageNum > 0)
        pagenumBlackStr = QString("%1").arg(lalipayBlackInfo.pageNum-1);
    else
        pagenumBlackStr = QString("%1").arg(lalipayBlackInfo.pageNum);
    qDebug(qPrintable(currentBlackStr));
    qDebug(qPrintable(pagenumBlackStr));

    if(currentBlackStr == pagenumBlackStr)
    {
        QString version;
        configSet.readSetInfo(configFile, Aliapygroup, "blacklistVerTemp", version);
        blacklistVer = version;
        configSet.writeSetInfo(configFile, Aliapygroup, "blacklistVer", blacklistVer);
        configSet.writeSetInfo(configFile, Aliapygroup, "blackCurrentNum", "0");
        isAskBlack = true;
        tradeScanTimer->start();
#ifdef ALIPAYBLACK_DEBUG
        qDebug("kira --- v2 Updata Alipay black success!");
        QMap<int, QString>::iterator blackInfo; //遍历map
        for(blackInfo=myblackMap.begin(); blackInfo!=myblackMap.end(); ++blackInfo)
        {
            qDebug("%d: %s", blackInfo.key(), blackInfo.value().toLatin1().data());
        }
#endif
    }
    else
    {
        QString ncurrentBlackStr = QString("%1").arg(lalipayBlackInfo.currentNum + 1);
        configSet.writeSetInfo(configFile, Aliapygroup, "blackCurrentNum", ncurrentBlackStr);
        askAlipayBlacklist();
        myTimer->start();
    }
}

void myPosWidget::processUnionpayAck_v1(unionPayAck_t tag)
{
    printf("qwy---processUnionpayAck_v1 %s %s\n",tag.cardNo,tag.tradeTime);
    QString cardNo = QByteArray::fromRawData(tag.cardNo,sizeof(tag.cardNo));
    QString tradetime = QByteArray::fromRawData(tag.tradeTime,sizeof(tag.tradeTime));
    scanPosDB->updateUnionpayInfoServerAck(cardNo,tradetime);
}

void myPosWidget::processAlipayKeyInfo_v1()
{
    myTimer->stop();
    alipayKeyInfo_t lalipayKeyInfo;
    //scanPosGprsTalk->readAlipayKeyInfo(&lalipayKeyInfo);
    scanPosNetTalk->readAlipayKeyInfo(&lalipayKeyInfo);
    isAskKey = false;
    for(int i=0; i<lalipayKeyInfo.num; i++)
    {
        QMap<int, QString>::iterator it = mykeyMap.find(lalipayKeyInfo.key[i].keyOrder);
        QString valueStr = QByteArray::fromRawData((char*)lalipayKeyInfo.key[i].keyValue, sizeof(lalipayKeyInfo.key[i].keyValue));
        if(it != mykeyMap.end())
        {
            //qDebug(qPrintable(valueStr));
            //qDebug(qPrintable(it.value()));
            if(valueStr != it.value())
            {
                //qDebug("kira --- updata mykeyMap: %d", lalipayKeyInfo.key[i].keyOrder);
                //mykeyMap[lalipayKeyInfo.key[i].keyOrder] = it.value();
                mykeyMap.insert(lalipayKeyInfo.key[i].keyOrder, valueStr);

            }
            // updata
            //scanPosDB->updataAlipayKey(lalipayKeyInfo.key[i].keyOrder, lalipayKeyInfo.key[i].keyValue);
            scanPosDB->updataAlipayKey(lalipayKeyInfo.key[i].keyOrder, valueStr);
        }
        else
        {
            mykeyMap.insert(lalipayKeyInfo.key[i].keyOrder, valueStr);
            // insert
            //scanPosDB->insertAlipayKey(lalipayKeyInfo.key[i].keyOrder, lalipayKeyInfo.key[i].keyValue);
            scanPosDB->insertAlipayKey(lalipayKeyInfo.key[i].keyOrder, valueStr);
        }
    }

    qDebug("kira --- Updata Alipay key success!");
    QMap<int, QString>::iterator keyInfo; //遍历map
    for(keyInfo=mykeyMap.begin(); keyInfo!=mykeyMap.end(); ++keyInfo)
    {
        qDebug("%d: %s", keyInfo.key(), keyInfo.value().toLatin1().data());
    }

    QString currentKeyStr = QString("%1").arg(lalipayKeyInfo.currentNum);
    //QString pagenumKeyStr = QString("%1").arg(lalipayKeyInfo.pageNum-1);
    QString pagenumKeyStr;
    if(lalipayKeyInfo.pageNum > 0)
        pagenumKeyStr = QString("%1").arg(lalipayKeyInfo.pageNum-1);
    else
        pagenumKeyStr = QString("%1").arg(lalipayKeyInfo.pageNum);
    qDebug(qPrintable(currentKeyStr));
    qDebug(qPrintable(pagenumKeyStr));

    if(currentKeyStr == pagenumKeyStr)
    {
        QString version;
        configSet.readSetInfo(configFile, Aliapygroup, "keylistVerTemp", version);
        keylistVer = version;
        configSet.writeSetInfo(configFile, Aliapygroup, "keylistVer", keylistVer);
        configSet.writeSetInfo(configFile, Aliapygroup, "keyCurrentNum", "0");
        isAskKey = true;
        tradeScanTimer->start();
    }
    else
    {
        QString ncurrentKeyStr = QString("%1").arg(lalipayKeyInfo.currentNum + 1);
        configSet.writeSetInfo(configFile, Aliapygroup, "keyCurrentNum", ncurrentKeyStr);
        askAlipayKeylist();
        myTimer->start();
    }

}

// v2 --- 2017.3.10
void myPosWidget::processAlipayCardType_v1()
{

}

void myPosWidget::processAlipayTradeAck_v1()
{
    alipayTradeInfoAck_t lalipayTradeInfoAck;
    //scanPosGprsTalk->readAlipayTradeAckInfo(&lalipayTradeInfoAck);
    scanPosNetTalk->readAlipayTradeAckInfo(&lalipayTradeInfoAck);
    QString tradeTimeStr = QByteArray::fromRawData((char*)lalipayTradeInfoAck.tradeTime, sizeof(lalipayTradeInfoAck.tradeTime));
    QString tradeRecordStr = QByteArray::fromRawData((char*)lalipayTradeInfoAck.tradeRecord, sizeof(lalipayTradeInfoAck.tradeRecord));
    QString userIDStr = QByteArray::fromRawData((char*)lalipayTradeInfoAck.userID, sizeof(lalipayTradeInfoAck.userID));
    qDebug("kira --- processAlipayTradeAck_v1:");
    qDebug(qPrintable(tradeTimeStr));
    qDebug(qPrintable(tradeRecordStr));
    qDebug(qPrintable(userIDStr));
    scanPosDB->updataAlipayServerAckV2(userIDStr, tradeTimeStr, tradeRecordStr);
}

void myPosWidget::processLongpress()
{
    switch(myui)
    {
        case signalUI:
            //scanPosDB->clearAlipayBlack();
            break;
        case buslineUI:

            break;
        case busidUI:
            scanPosBusline->keyboardShortpress();
            break;
        case posidUI:
            scanPosBusID->keyboardShortpress();
            break;
        case blackclearUI:
            scanPosPosID->keyboardShortpress();
            break;
    }
    ui->myStackedWidget->setCurrentWidget(scanPosTalk);
    myui = signalUI;
}

void myPosWidget::processShortpress()
{
    switch(myui)
    {
        case signalUI:
            //scanPosDB->clearAlipayBlack();
            ui->myStackedWidget->setCurrentWidget(scanPosSignal);
            myui = buslineUI;
            break;
        case buslineUI:
            ui->myStackedWidget->setCurrentWidget(scanPosBusline);
            myui = busidUI;
            break;
        case busidUI:
            scanPosBusline->keyboardShortpress();
            ui->myStackedWidget->setCurrentWidget(scanPosBusID);
            myui = posidUI;
            break;
        case posidUI:
            scanPosBusID->keyboardShortpress();
            ui->myStackedWidget->setCurrentWidget(scanPosPosID);
            myui = blackclearUI;
            break;
        case blackclearUI:
            scanPosPosID->keyboardShortpress();
            ui->myStackedWidget->setCurrentWidget(scanPosBlackClear);
            myui = signalUI;
            break;
    }
}

void myPosWidget::processLeftpress()
{
    qDebug("kira --- process left key press...");
    switch(myui)
    {
        case signalUI:

            break;
        case buslineUI:

            break;
        case busidUI:
            scanPosBusline->keyboardLeftpress();
            break;
        case posidUI:
            scanPosBusID->keyboardLeftpress();
            break;
        case blackclearUI:
            scanPosPosID->keyboardLeftpress();
            break;
    }
}

void myPosWidget::processRightpress()
{
    qDebug("kira --- process right key press...");
    switch(myui)
    {
        case signalUI:

            break;
        case buslineUI:

            break;
        case busidUI:
            scanPosBusline->keyboardRightpress();
            break;
        case posidUI:
            scanPosBusID->keyboardRightpress();
            break;
        case blackclearUI:
            scanPosPosID->keyboardRightpress();
            break;
    }
}

void myPosWidget::processCheckConfirmBusID()
{
    scanPosBusID->getBusID(busID);
    scanPosTalk->setBusID(busID);
    configSet.writeSetInfo(configFile, workgroup, "busID", busID);
}

void myPosWidget::processCheckConfirmPosID()
{
    scanPosPosID->getPosID(posID);
    configSet.writeSetInfo(configFile, workgroup, "posID", posID);
}

void myPosWidget::processCheckConfirmBusline()
{
    scanPosBusline->getBusline(buslineID);
    scanPosBusline->getBusticket(tickets);
    scanPosTalk->setTickets(tickets);
    scanPosStmTalk->setTickets(tickets);
    configSet.writeSetInfo(configFile, workgroup, "buslineID", buslineID);
    configSet.writeSetInfo(configFile, workgroup, "tickets", tickets);
}

void myPosWidget::processDriveSign()
{
    driveSignRecordInfo_t lsignInfo;
    scanPosScanTalk->getDriveSignRecordInfo(&lsignInfo);

    // dsa verity
    unsigned char signHexLen =
            (unsigned char)(((Hex_of_Char(lsignInfo.signInfo.signLen[0])<<4) & 0xf0)
                            + (Hex_of_Char(lsignInfo.signInfo.signLen[1]) & 0x0f));
    signHexLen = (signHexLen/16)*10 + (signHexLen%16);
    qDebug("kira --- drive signlen = %d", signHexLen);
    unsigned char signHex[signHexLen];
    for(int p=0; p<signHexLen; p++)
    {
        signHex[p] =
                (unsigned char)(((Hex_of_Char(lsignInfo.signInfo.signInfo[2*p])<<4) & 0xf0)
                                +(Hex_of_Char(lsignInfo.signInfo.signInfo[2*p+1]) & 0x0f));
    }
    int signInfoLen = sizeof(lsignInfo.signInfo.head)+sizeof(lsignInfo.signInfo.driveID);
    unsigned char signInfo[signInfoLen];
    memcpy(signInfo,
           (unsigned char*)lsignInfo.signInfo.head,
           sizeof(lsignInfo.signInfo.head));
    memcpy(&signInfo[sizeof(lsignInfo.signInfo.head)],
           (unsigned char*)lsignInfo.signInfo.driveID,
           sizeof(lsignInfo.signInfo.driveID));

    int ret = scanPosScanTalk->dsaVerify(signHex, signHexLen, signInfo, signInfoLen);
    qDebug("kira --- drive sign dsaVerity ret = %d", ret);

    if(ret == 1)
    {
        qDebug("kira --- drive sign dsaverity success...");
        driverID = QByteArray::fromRawData((char*)lsignInfo.signInfo.driveID, sizeof(lsignInfo.signInfo.driveID));
        //可添加防误刷机制
        bool isRepeat = scanPosDB->judgeDriverSignRepeat(driverID);
        if(isRepeat)
        {
            qDebug("kira --- driver scan card in 10s; please wait 10s...");
        }
        else
            dealDriveSign();
    }
    else
    {
        qDebug("kira --- drive sign dsaverity failed...");
        scanPosStmTalk->stmVoiceCmd(19);
    }
}

void myPosWidget::dealDriveSign()
{
    driveSignNetInfo_t lsignNetInfo;
    memset(&lsignNetInfo, 0, sizeof(driveSignNetInfo_t));
    QString lDateTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");

    memcpy(lsignNetInfo.posID, posID.toLatin1().data(), posID.length());
    memcpy(lsignNetInfo.driveID, driverID.toLatin1().data(), driverID.length());
    memcpy(lsignNetInfo.signTime, lDateTime.toLatin1().data(), lDateTime.length());
    memcpy(lsignNetInfo.busline, buslineID.toLatin1().data(), buslineID.length());
    memcpy(lsignNetInfo.busID, busID.toLatin1().data(), busID.length());
    memcpy(lsignNetInfo.cityID, cityID.toLatin1().data(), cityID.length());

    // location fill test data
    //memcpy(lsignNetInfo.longitude, "120.123E", sizeof(lsignNetInfo.longitude));
    //memcpy(lsignNetInfo.latitude, "30.3452N", sizeof(lsignNetInfo.latitude));

    // location fill data
    gpsSerialInfo gpsInfo;
    scanPosGpsTalk->getGpsInfo(&gpsInfo);
    QString longitudeStr = QByteArray::fromRawData((char*)gpsInfo.longitude, 12);
    qDebug("kira --- longitudeStr:");
    qDebug(qPrintable(longitudeStr));
    QString latitudeStr = QByteArray::fromRawData((char*)gpsInfo.latitude, 12);
    qDebug("kira --- latitudeStr:");
    qDebug(qPrintable(latitudeStr));
    memcpy(lsignNetInfo.longitude, longitudeStr.toLatin1().data(), sizeof(lsignNetInfo.longitude));
    memcpy(lsignNetInfo.latitude, latitudeStr.toLatin1().data(), sizeof(lsignNetInfo.latitude));

    bool ret = scanPosDB->processDriverSign(lsignNetInfo);

    if(ret)
    {
        scanPosStmTalk->stmVoiceCmd(25);
        qDebug("kira --- signFlag: %c", lsignNetInfo.signFlag);
        if('1' == lsignNetInfo.signFlag)
        {
            qDebug("kira --- driverID: ");
            qDebug(qPrintable(driverID));
            scanPosTalk->setdriverID(driverID);
        }
        else
        {
            scanPosTalk->setdriverID(" ");
            driverID = " ";
            qDebug(qPrintable(driverID));
            // 是否需要签退后数码管“清零”
            scanPosStmTalk->stmClearLEDDisp();
        }
        // 组包
        int datalen = sizeof(driveSignNetInfo_t);
        unsigned char signData[1024] = {0};
        memset(signData, 0, sizeof(signData));
        signData[0] = 0x55;
        signData[1] = 0x7a;
        signData[2] = 0x29;
        memcpy(&signData[3], &datalen, 2);
        signData[5] = 0x00;
        memcpy(&signData[6], &lsignNetInfo, datalen);
        unsigned char check = 0;
        for(int i=0; i<datalen+6; i++)
            check ^= signData[i];
        signData[datalen+6] = check;

        //ret = scanPosGprsTalk->sendGprsData((char*)signData, 7+datalen);
        int sendlen;
        ret = scanPosNetTalk->socketSend(signData, 7+datalen, &sendlen);
        if(1 == ret)
        {
            qDebug("kira --- 发送司机签到记录失败...");
        }
    }
    else
    {
        scanPosStmTalk->stmVoiceCmd(10);
        qDebug("kira --- 记录司机签到失败...");
    }
}

void myPosWidget::processDriverSignAck_v1()
{
    driveSignNetAck_t ldriveSignNetAck;
    //scanPosGprsTalk->readDriverSignInfo(&ldriveSignNetAck);
    scanPosNetTalk->readDriverSignInfo(&ldriveSignNetAck);
    QString signtimeStr = QByteArray::fromRawData((char*)ldriveSignNetAck.signTime, sizeof(ldriveSignNetAck.signTime));
    QString driverIDStr = QByteArray::fromRawData((char*)ldriveSignNetAck.driveID, sizeof(ldriveSignNetAck.driveID));
    scanPosDB->updataDriverSignAck(driverIDStr, signtimeStr);
}

void myPosWidget::processUnionPayTradeInfo()
{
    cardreceiveA2 lunionPayProcA2;
    scanPosStmTalk->getCardTradeInfo(&lunionPayProcA2);

    //unionpay record pre
    unionPayInfo_t tag;
    memset(&tag, 0, sizeof(unionPayInfo_t));
    memcpy(tag.busline,buslineID.toLatin1().data(),buslineID.length());
    memcpy(tag.busID,busID.toLatin1().data(),busID.length());
    memcpy(tag.posID,posID.toLatin1().data(),posID.length());
    memcpy(tag.cityID,cityID.toLatin1().data(),cityID.length());
    memcpy(tag.driverID,driverID.toLatin1().data(),driverID.length());
    QString currentDateTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    memcpy(tag.tradeTime, currentDateTime.toLatin1().data(), currentDateTime.length());
    int tmp = (int)(tickets.toDouble() * 100.0);
    memcpy(tag.ticketPrice, &tmp, 4);
    gpsSerialInfo tagGps;
    scanPosGpsTalk->getGpsInfo(&tagGps);
//    printf("qwy---%s %s \n",tagGps.longitude,tagGps.latitude);
    memcpy(tag.longitude, tagGps.longitude, sizeof(tag.longitude));
    memcpy(tag.latitude, tagGps.latitude, sizeof(tag.latitude));
    memcpy(tag.cardNo,lunionPayProcA2.cardno,sizeof(tag.cardNo));
    memcpy(tag.serialNo,lunionPayProcA2.serialno,sizeof(tag.serialNo));
    memcpy(tag.validDate,lunionPayProcA2.validdate,sizeof(tag.validDate));
    tag.tradeType = lunionPayProcA2.tradetype;
    QString currentYear = QDateTime::currentDateTime().toString("yyyy");
    memcpy(tag.settlementDate,currentYear.toLatin1().data(),currentYear.length());
    memcpy(tag.cardCenterTime,currentYear.toLatin1().data(),currentYear.length());
    // 组包
    int datalen = sizeof(unionPayInfo_t);
    unsigned char tradeData[512] = {0};
    tradeData[0] = 0x55;
    tradeData[1] = 0x7a;
    //unionpay
//    TimerTesting tt;
    int ret = scanPosUnionPay->processA2Talk(&tag,lunionPayProcA2, (int)(tickets.toDouble()*100));
//    tt.PrintlnTimeUse();
    if(0 == ret)  /// kira - test
    {
        int amount = (int)(tickets.toDouble() * 100.0);
        QString amountInteger = QString::number(amount/100, 10);
        int amountInt = amount%100;
        QString amountDecimalH = QString::number(amountInt/10, 10);
        QString amountDecimalL = QString::number(amountInt%10, 10);
        debit.clear();
        debit.append(amountInteger);
        debit.append(".");
        debit.append(amountDecimalH);
        debit.append(amountDecimalL);

        int intBalance = 0;
        scanPosUnionPay->getUnionPayTradeRet(&intBalance);
        QString balanceInteger = QString::number(intBalance/100, 10);
        int balanceInt = intBalance%100;
        QString balanceDecimalH = QString::number(balanceInt/10, 10);
        QString balanceDecimalL = QString::number(balanceInt%10, 10);
        balance.clear();
        balance.append(balanceInteger);
        balance.append(".");
        balance.append(balanceDecimalH);
        balance.append(balanceDecimalL);

        scanPosStmTalk->setBalance(balance);
        scanPosStmTalk->setDebit(debit);
        //scanPosStmTalk->stmVoiceCmd(12);
        scanPosTradeTalk->setBalance(balance);
        scanPosTradeTalk->setDebit(debit);
        scanPosScanTalk->tradeRledCtrl(0);
        scanPosScanTalk->tradeGledCtrl(1);

        if(isShowPosWidget)
        {
            isShowPosWidget = false;
            ui->myStackedWidget->setCurrentWidget(scanPosTradeTalk);
            //启动定时器
            scanPosTradeTalk->myTimer->start();
        }
        else
        {
            //重置定时器
            scanPosTradeTalk->myTimer->stop();
            scanPosTradeTalk->myTimer->start();
        }
        //write to sqlite and tcp package
        tag.tradeNature = 0;
        if(!scanPosDB->insertUnionpayInfo(tag)){
            printf("qwy --- insert unionpay info is fail\n");
            return;
        }
        // 组包
        tradeData[2] = 0x36;
    }
    else
    {
        if(false == failTimer->isActive())
        {
            failTimer->start();
            scanPosScanTalk->tradeRledCtrl(1);
            scanPosScanTalk->tradeGledCtrl(0);
        }
        //write to sqlite and tcp package
        tag.tradeNature = 1;
        if(!scanPosDB->insertUnionpayInfo(tag)){
            printf("qwy --- insert unionpay info is fail\n");
            return;
        }
        // 组包
        tradeData[2] = 0x37;
    }
    memcpy(&tradeData[3], &datalen, 2);
    tradeData[5] = 0x00;
    memcpy(&tradeData[6], &tag, sizeof(unionPayInfo_t));
    unsigned char check = 0;
    for(int i=0; i<datalen+6; i++)
        check ^= tradeData[i];
    tradeData[datalen+6] = check;
    int sendlen;
    ret = scanPosNetTalk->socketSend(tradeData, 7+datalen, &sendlen);
    if(1 == ret)
    {
        //scanPosStmTalk->stmVoiceCmd(10);
        qDebug("qwy --- 发送银联交易记录失败...");
    }
#if 0
    printf("qwy---unionpay data:");
    for(int i=0;i<datalen+7;++i){
        printf("%02x ",tradeData[i]);
    }
    printf("\n");
#endif
}

void myPosWidget::processUnionPayTermMkInfo()
{
    QString ltermMk;
    scanPosStmTalk->getTermMk(ltermMk);
    scanPosUnionPay->recordTermMk(ltermMk);
    scanPosStmTalk->stmVoiceCmd(12);
}

// pos param
void myPosWidget::processPosParamSet()
{
    posParam_t posParam;
    scanPosNetTalk->readPosParamInfo(&posParam);
    QString posIDstr = QByteArray::fromRawData((char*)posParam.posID, sizeof(posParam.posID));
    QString buslineIDstr = QByteArray::fromRawData((char*)posParam.buslineID, sizeof(posParam.buslineID));
    QString busIDstr = QByteArray::fromRawData((char*)posParam.busID, sizeof(posParam.busID));
    QString cityIDstr = QByteArray::fromRawData((char*)posParam.cityID, sizeof(posParam.cityID));
    int itickets = 0;
    memcpy(&itickets, posParam.tickets, 2);
    qDebug("kira --- pos param set tickets = %d", itickets);
    QString ticketsstr = QString::number(itickets, 10);

    configSet.writeSetInfo(configFile, workgroup, "busID", busIDstr);
    configSet.writeSetInfo(configFile, workgroup, "buslineID", buslineIDstr);
    //configSet.writeSetInfo(configFile, workgroup, "posID", posIDstr);
    configSet.writeSetInfo(configFile, workgroup, "tickets", ticketsstr);
    configSet.writeSetInfo(configFile, workgroup, "cityID", cityIDstr);

    ackPosParamSet();
}

void myPosWidget::ackPosParamSet()
{
    configSet.readSetInfo(configFile, workgroup, "busID", busID);
    configSet.readSetInfo(configFile, workgroup, "buslineID", buslineID);
    configSet.readSetInfo(configFile, workgroup, "posID", posID);
    configSet.readSetInfo(configFile, workgroup, "tickets", tickets);
    configSet.readSetInfo(configFile, workgroup, "cityID", cityID);

    posParam_t l_posParam;
    int sendlen;
    unsigned char posParamData[7+sizeof(posParam_t)] = {0};
    posParamData[0] = 0x55;
    posParamData[1] = 0x7a;
    posParamData[2] = 0x31;
    int posParamLen = sizeof(posParam_t);
    memcpy(&posParamData[3], &posParamLen, 2);
    posParamData[5] = 0x00;

    memcpy(&l_posParam.posID, posID.toLatin1().data(), sizeof(l_posParam.posID));
    memcpy(&l_posParam.posID, busID.toLatin1().data(), sizeof(l_posParam.busID));
    memcpy(&l_posParam.posID, buslineID.toLatin1().data(), sizeof(l_posParam.buslineID));
    memcpy(&l_posParam.posID, tickets.toLatin1().data(), sizeof(l_posParam.tickets));
    memcpy(&l_posParam.posID, cityID.toLatin1().data(), sizeof(l_posParam.cityID));
    QString dateTimestr = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    memcpy(&l_posParam.time, dateTimestr.toLatin1().data(), sizeof(l_posParam.time));

    memcpy(&posParamData[6], &l_posParam, sizeof(posParam_t));
    unsigned char check = 0;
    for(int i=0; i<sizeof(posParam_t)+6; i++)
        check ^= posParamData[i];
    posParamData[sizeof(posParam_t)+6] = check;
    scanPosNetTalk->socketSend(posParamData, 7+posParamLen, &sendlen);
}

// ftp info
void myPosWidget::processPosFtpInfo()
{
    posUpdataInfo_t posUpdataInfo;
    scanPosNetTalk->readFtpInfo(&posUpdataInfo);

    QString ftpInfoStr = QByteArray::fromRawData(posUpdataInfo.updataInfo, posUpdataInfo.infoLen);

    qDebug("kira --- ftp info: ");
    qDebug(qPrintable(ftpInfoStr));

    QStringList ftpInfoList = ftpInfoStr.split(";");
    posFtpInfo_t posFtpInfo;
    if(ftpInfoList.size() >= 7)
    {
        posFtpInfo.version = ftpInfoList.at(0);
        posFtpInfo.ftpIp = ftpInfoList.at(1);
        posFtpInfo.ftpPort = ftpInfoList.at(2);
        posFtpInfo.ftpPath = ftpInfoList.at(3);
        posFtpInfo.ftpUser = ftpInfoList.at(4);
        posFtpInfo.ftpPassword = ftpInfoList.at(5);
        posFtpInfo.crc = ftpInfoList.at(6);
        qDebug("kira --- ftp set info: ");
        qDebug(qPrintable(posFtpInfo.version));
        qDebug(qPrintable(posFtpInfo.ftpIp));
        qDebug(qPrintable(posFtpInfo.ftpPort));
        qDebug(qPrintable(posFtpInfo.ftpPath));
        qDebug(qPrintable(posFtpInfo.ftpUser));
        qDebug(qPrintable(posFtpInfo.ftpPassword));
        qDebug(qPrintable(posFtpInfo.crc));

        // v2 --- 2017.3.14
        appUpdaterError upRet;
        upRet = myAppUpdater.processUpdate(posFtpInfo, scanPosStmTalk);
        if(noError == upRet)
        {
            ;
        }
        else
        {
            judgeFtpWork = false;
        }
    }
}

int myPosWidget::Hex_String_to_Bytes(
        char* hex_string,
        int hex_string_len,
        unsigned char* bytes,
        int bytes_len)
{
    int i = 0;

    if((hex_string_len % 2 != 0) || (bytes_len * 2 < hex_string_len))
    {
        return -1;
    }

    for(i = 0; i < hex_string_len; i += 2)
    {
        bytes[i/2] = ((Hex_of_Char(hex_string[i]) << 4) & 0xF0) |
                    (Hex_of_Char(hex_string[i + 1]) & 0x0F);
    }
    return 1;
}

unsigned char myPosWidget::Hex_of_Char(char c)
{
    unsigned char tmp = 0;
    if(c >= '0' && c <= '9')
    {
        tmp = (c - '0');
    }
    else if(c >= 'A' && c <= 'F')
    {
        tmp = (c - 'A' + 10);
    }
    else if(c >= 'a' && c <= 'f')
    {
        tmp = (c - 'a' + 10);
    }
    return tmp;
}

#define QRCODE_HEX_DATA  "0101003E013230383831303231353038343233333177000000FFFFFFFF0000FFFF00FFFFFFFF1E00000228FF6B90D4EABC323D4BEA4BFFADC3D5CFCE63EEFBE30CC246304402206F20A77F52D511C6129854D251F4E6BD407F4FBB61EE06968FD73F1B42072FC602205CEFF6AB6E6D4F65241B986C10E8AC0F190D79C6FC4443FE5EAE987FE30642FD04575D0118383036021900FB52216F51700AE97D65523D6B95DBE58E400996AEE27A850219008ED68202B3698F47973D15593054B7B15BEC0B03F80706D3"
void myPosWidget::mock_qrcode(unsigned char *qrcode, int *qrcode_len)
{
    char qrcode_hex[] = QRCODE_HEX_DATA;
    int qrcode_hex_len = strlen(qrcode_hex);
    *qrcode_len = strlen(qrcode_hex)/2;
    Hex_String_to_Bytes(qrcode_hex, qrcode_hex_len, qrcode, *qrcode_len);
}
