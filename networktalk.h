#ifndef NETWORKTALK_H
#define NETWORKTALK_H

#include <QObject>
#include <QSocketNotifier>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/vfs.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <QMutex>
#include <QMutexLocker>
#include <QThread>
#include "serialdata.h"
#include "datastructure.h"
#include "unionpaynet.h"

class QTimer;

#define SERVERDATA_LEN  255
#define RECVDATA_LEN    4096

class NetworkTalk : public QThread
{
    Q_OBJECT
public:
    explicit NetworkTalk();

public:
    int socketConnect();
    int socketSend(unsigned char *data, int len, int *hadsend);
    int protocol01_get_len(unsigned char *data, int datalen);
    int protocol01_decode(unsigned char *data, int datalen, unsigned char *cmd, unsigned char **realdata, int *realdatalen);
    int analysecmd01(int cmd, unsigned char *realdata, int datalen);
    void setServerIP(QString sip) { serverIP = sip; }
    void setServerPort(QString sport) { serverPort = sport; }
    void fillServerData(char* pdata, int dataLen);
    void readServerData(char* pdata, int* pdataLen);
    void fillBeatAckInfo(char* pdata, int dataLen);
    void readBeatAckInfo(beatAckInfo_t* pbeatAckInfo);
    void fillConsumeAckInfo(char* pdata, int dataLen);
    void readConsumeAckInfo(consumeAckInfo_t* pconsumeAckInfo);
    void gprsLedCtrl(int flag);
    void powerCtrl(int flag);
    void powerctrlCtrl(int flag);
    void rstCtrl(int flag);

    // 微信在线支付
    void fillweixinTradeAckInfo(char* pdata, int dataLen);
    void readweixinTradeAckInfo(weixinTradeAck_t* pweixinTradeAck);
    void fillweixinBeatAckInfo(char* pdata, int dataLen);
    void readweixinBeatAckInfo(weixinBeatAck_t* pweixinBeatAck);
    void fillOnlineTradeAckInfo(char* pdata, int dataLen);
    void readOnlineTradeAckInfo(onlineTradeAck_t* ponlineTradeAck);
    // Alipay
    void fillAlipayKeyInfo(char* pdata, int dataLen);
    void readAlipayKeyInfo(alipayKeyInfo_t* palipayKeyInfo);
    void fillAlipayTradeAckInfo(char* pdata, int dataLen);
    void readAlipayTradeAckInfo(alipayTradeInfoAck_t* palipayTradeInfoAck);
    void fillAlipayBlackInfo(char* pdata, int dataLen);
    void readAlipayBlackInfo(alipayBlackInfo_t* palipayBlackInfo);
    // driver sign
    void fillDriverSignInfo(char* pdata, int dataLen);
    void readDriverSignInfo(driveSignNetAck_t* pdriveSignNetAck);
    // pos param
    void fillPosParamInfo(char* pdata, int dataLen);
    void readPosParamInfo(posParam_t* pposParam);
    // dtp info
    void fillFtpInfo(char* pdata, int dataLen);
    void readFtpInfo(posUpdataInfo_t* pposUpdataInfo);
    void dealGprsTalkError();
    // up
    void fillUpdataAckInfo(char* pdata, int dataLen);
    void readUpdataAckInfo(updataInfpAck_t* pupdataInfpAck);

signals:
    void sendBeatInfo();
    void recvBeatAck_v1();
    void recvConsumeAck_v1();
    void recvWeixinTradeAck_v1();
    void recvWeixinBeatAck_v1();
    void recvAlipayTradeAck_v1();
    void recvAlipayKeyInfo_v1();
    void recvAlipayBlackInfo_v1();
    void recvDriverSignInfo_v1();
    // pos param
    void recvPosParamsetInfo();
    // ftp info
    void recvFtpInfo();
	void recvUnionpayAck_v1(unionPayAck_t);
    // up
    void recvUpdataInfoAck();

public slots:
    void beatTimerOut();
    void gprsLedTimerOut();
    char AscToHex(char aChar);

protected:
    void run();

public:
    void stop();

private:
    bool m_stopFlag;
    QMutex threadMutex;

public:
    bool openSerial();
    SerialData gprsSerialData;
    QSocketNotifier* gprsMonitor;
    int socketfd;
    int powerfd;
    int powerctrlfd;
    int gprsLedfd;
    int rstfd;
    char gprsbuf[RECVDATA_LEN];
    char gprsData[RECVDATA_LEN];
    char gprsCmd[512];
    int recvLen;
    int gprsDatalen;
    int gprsCmdlen;
    int recvDatalen;
    bool gprsflag;
    bool gprsPowerFlag;
    gprsState gprsInit;
    QTimer *myTimer;        // GPRS网络定时检测定时器
    QTimer *beatTimer;      // 心跳信息定时器
    QTimer *gprsLedTimer;
    bool gprsLedFlag;
    QString serverIP;
    QString serverPort;
    int recvDataErrorTime;
    int recvCmdErrorTime;
    int initGprsTime;
    int serverDataErrorTime;
    //char serverData[SERVERDATA_LEN];
    //int serverDataLen;
    beatAckInfo_t beatAckInfo;
    consumeAckInfo_t consumeAckInfo;
    QMutex beatMutex;
    QMutex consumeMutex;
    QMutex alipaykeyMutex;
    QMutex alipayblackMutex;
    QMutex alipaytradeMutex;
    QMutex driverSignMutex;

    // 微信在线支付
    weixinTradeAck_t weixinTradeAck;
    weixinBeatAck_t weixinBeatAck;
    onlineTradeAck_t onlineTradeAck;

    // Alipay
    alipayKeyInfo_t alipayKeyInfo;
    alipayBlackInfo_t alipayBlackInfo;
    alipayTradeInfoAck_t alipayTradeInfoAck;

    // driver sign
    driveSignNetAck_t driveSignNetAck;

    // pos param
    posParam_t posParam;

    // ftp info
    posUpdataInfo_t posUpdataInfo;

    // up
    updataInfpAck_t updataInfpAck;

    UnionPayNet network;
};

#endif // NETWORKTALK_H
