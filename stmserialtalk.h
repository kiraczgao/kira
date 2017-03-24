#ifndef STMSERIALTALK_H
#define STMSERIALTALK_H

#include <QWidget>
#include <QSocketNotifier>
#include <QMessageBox>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "serialdata.h"
#include "unionpaystructure.h"
#include <string.h>
#include <stdio.h>
#include <QMutex>
#include <QMutexLocker>
#include <QThread>
#include "datastructure.h"

const char head_f = 0x55;
const char head_s = 0x7a;
const int headlen = 6;

//class stmSerialTalk : public QWidget
class stmSerialTalk : public QThread
{
    Q_OBJECT
public:
    //explicit stmSerialTalk(QWidget *parent = 0);
    explicit stmSerialTalk();

signals:
    void recvCardInfo();
    void recvLongpress();
    void recvShortpress();
    void recvLeftpress();
    void recvRightpress();
    void recvCardTradeInfoA2();
    void recvTermMkInfoA3();
    void recvPscamAck(char);    // v2 --- 2017.3.15

public:
    void stmVoiceCmd(char type);
    void stmVoiceCmdDebit(char cmd);
    void stmVoiceRescan(char cmd);
    void stmVoiceTrade(char cmd);
    void stmVoiceWait(char cmd);
    void stmVoiceQRError(char cmd);
    void stmVoiceBlack(char cmd);
    void stmVoiceUseBusQR(char cmd);
    void stmVoiceDriverSign(char cmd);
    void stmClearLEDDisp();
    void stmSendPsamInfo(pscamInfo_t l_pscamInfo);
    void stmSendData(unsigned char command, char* pdata, int len);

public:
    void setBalance(const QString& sbalance) { balance = sbalance; }
    void getBalance(QString& gbalance) { gbalance = balance; }
    void setDebit(const QString& sdebit) { debit = sdebit; }
    void getDebit(QString& gdebit) { gdebit = debit; }
    void setTickets(const QString stickets) { tickets = stickets; }
    void getTermMk(QString& gtermMk) { gtermMk = mytermMk; }

    int checkHead(char* pdata, int* datalen, char* cmd);
    int checkVertify(char* pdata, int datalen);
    void processPress(char* pdata, int datalen);
    void processCardTradeInfo(char* pdata, int datalen);
    void getCardTradeInfo(cardreceiveA2* pcardreceiveA2);
    unsigned char HexToAsc(unsigned char aHex);
    unsigned char AscToHex(unsigned char aChar);
    void processUnionPayTermMkInfo(char* pdata, int datalen);

public slots:
    void readData();
    void writeData(char* data, int datalen);
    void processUnionPayProcA1();
    void unionPayTradeSuccess();
    void unionPayTradeFailed();
    void processUnionPayProcA2();

public:
    bool openSerial();
    SerialData stmSerialData;
    QSocketNotifier* stmMonitor;
    int stmfd;
    char stmbuf[1024];
    int recvLen;
    cardreceiveA2 cardTradeInfoA2;
    QString mytermMk;

public:
    QString debit;
    QString balance;
    QString tickets;

protected:
    void run();

public:
    void stop();

private:
    bool m_stopFlag;
    QMutex threadMutex;
};

#endif // STMSERIALTALK_H
