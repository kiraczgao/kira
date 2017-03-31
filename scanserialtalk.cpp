#include "scanserialtalk.h"
#include <QDebug>
#include <QTimer>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#if 1
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/dsa.h>
#endif

#define OPENSSLKEY 	"/opt/kira/key/dsakey-pri.pem"
#define PUBLICKEY 	"/opt/kira/key/publicKey.pem"
#define BUFFSIZE 	1024
//#define SCAN_DEBUG

//scanSerialTalk::scanSerialTalk(QWidget *parent) : QWidget(parent)
scanSerialTalk::scanSerialTalk()
{
    qDebug("kira --- Initial scan");
    scanSerialData.devName = "ttySP1";
    scanSerialData.baudRate = "B9600";
    scanSerialData.dataBit = "8";
    scanSerialData.parity = "none";
    scanSerialData.stopBit = "1";
    recvLen = 0;
    m_stopFlag = false;
    memset(scanbuf, 0, sizeof(scanbuf));

    ledGfd = open("/sys/class/gpio/gpio108/value", O_RDWR);
    if(ledGfd < 0)
    {
        qDebug("kira --- open trade green led failed: %d", ledGfd);
    }

    ledRfd = open("/sys/class/gpio/gpio109/value", O_RDWR);
    if(ledRfd < 0)
    {
        qDebug("kira --- open trade red led failed: %d", ledRfd);
    }

    scanctlfd = open("/sys/class/gpio/gpio78/value", O_RDWR);
    if(scanctlfd < 0)
    {
        qDebug("kira --- open scan failed: %d", scanctlfd);
    }
    this->tradeScanCtrl(1);

    this->openSerial();

#if 1
    //新建一个QTimer对象
    myTimer = new QTimer();
    //设置定时器每个多少毫秒发送一个timeout()信号
    myTimer->setInterval(100);
    //启动定时器
    myTimer->stop();
    connect(myTimer, SIGNAL(timeout()), this, SLOT(onTimerOut()), Qt::DirectConnection);
#endif
}

bool scanSerialTalk::openSerial()
{
    struct termios setting;
    scanfd = open(qPrintable("/dev/" + scanSerialData.devName), O_RDWR);
    if (scanfd == -1) {
        //QMessageBox::warning(this, "scan open error", strerror(errno));
        goto openerr;
    }
    tcgetattr(scanfd, &setting);
    //  设置波特率
    speed_t speed;
    if (scanSerialData.baudRate == "B4800") {
        speed = B4800;
    } else if (scanSerialData.baudRate == "B9600") {
        speed = B9600;
    } else if (scanSerialData.baudRate == "B19200") {
        speed = B19200;
    } else if (scanSerialData.baudRate == "B38400") {
        speed = B38400;
    } else if (scanSerialData.baudRate == "B57600") {
        speed = B57600;
    } else if (scanSerialData.baudRate == "B115200") {
        speed = B115200;
    } else if (scanSerialData.baudRate == "B230400") {
        speed = B230400;
    } else if (scanSerialData.baudRate == "B460800") {
        speed = B460800;
    } else {
        //QMessageBox::warning(this, "error", "scan speed error!");
        goto seterr;
    }
    cfsetispeed(&setting, speed);
    cfsetospeed(&setting, speed);
    cfmakeraw(&setting);
    //  设置数据位
    if (scanSerialData.dataBit == "8") {
        setting .c_cflag |= CS8;
    } else if (scanSerialData.dataBit == "7") {
        setting.c_cflag |= CS7;
    } else if (scanSerialData.dataBit == "6") {
        setting.c_cflag |= CS6;
    } else if (scanSerialData.dataBit == "5") {
        setting.c_cflag |= CS5;
    } else {
        //QMessageBox::warning(this, "error", "scan dataBit error!");
        goto seterr;
    }
    //  设置parity
    if (scanSerialData.parity == "none") {
        setting.c_cflag &= ~PARENB;
        setting.c_iflag &= ~INPCK;
    } else if (scanSerialData.parity == "odd") {
        setting.c_cflag |= (PARODD | PARENB);
        setting.c_iflag |= INPCK;
    } else if (scanSerialData.parity == "even") {
        setting.c_cflag |= PARENB;
        setting.c_cflag &= ~PARODD;
        setting.c_iflag |= INPCK;
    } else {
        //QMessageBox::warning(this, "error", "scan dataBit error!");
        goto seterr;
    }
    //  设置停止位
    if (scanSerialData.stopBit == "1") {
        setting.c_cflag &= ~CSTOPB;
    } else if (scanSerialData.stopBit == "2") {
        setting.c_cflag |= CSTOPB;
    } else {
        //QMessageBox::warning(this, "error", "scan stopBit error!");
        goto seterr;
    }

    tcflush(scanfd, TCIFLUSH);
    setting.c_cc[VTIME] = 0;
    setting.c_cc[VMIN] = 0;
    tcsetattr(scanfd, TCSANOW, &setting);

#if 0
    scanMonitor = new QSocketNotifier(scanfd, QSocketNotifier::Read, NULL);
    connect(scanMonitor, SIGNAL(activated(int)), this, SLOT(readData()));
    scanMonitor->setEnabled(true);
#endif

    return true;
seterr:
    ::close(scanfd);
openerr:
    return false;
}

void scanSerialTalk::stop()
{
    QMutexLocker locker(&threadMutex);
    m_stopFlag = true;
}

void scanSerialTalk::run()
{
    int ret = 0;
    fd_set  sets;
    struct timeval val;

    while(1)
    {
        QMutexLocker locker(&threadMutex);
        FD_ZERO(&sets);
        FD_SET(scanfd, &sets);

        val.tv_sec = 0;
        val.tv_usec = 5000;
        ret = select(scanfd+1, &sets, NULL, NULL, &val);
        if (ret == -1)
        {
            ;
        }
        else if(ret)
        {
            usleep(20);
            if(FD_ISSET(scanfd, &sets))
            {
                readData();
            }
        }

        if(m_stopFlag)
            break;
    }
    m_stopFlag = false;
}

void scanSerialTalk::readData()
{
    myTimer->stop();

    //char* p;
    recvLen += read(scanfd, scanbuf+recvLen, sizeof(scanbuf));
#ifdef SCAN_DEBUG
    for(int i=0; i<recvLen; i++)
        printf("%.2x ", scanbuf[i]);
    printf("\n");
    qDebug("kira --- receive scanInfo over... len=%d", recvLen-2);
#endif
    //myTimer->start();
    if(false == myTimer->isActive())
    {
        myTimer->start();
    }

    //p = strstr(scanbuf, "\r\n");
    //if(p != NULL)
#if 0
    if(((recvLen-2) > 0) && (scanbuf[recvLen-2] == 0x0d) && (scanbuf[recvLen-1] == 0x0a))
    {

#ifdef SCAN_DEBUG
        QString recvData = QByteArray::fromRawData(scanbuf, recvLen-2);
        qDebug(qPrintable(recvData));
#endif
        qDebug("kira --- emit recvScanInfo()...");
        memcpy(alipayScanInfo.scanData, scanbuf, recvLen-2);
        alipayScanInfo.scanLen = recvLen - 2;
        memset(scanbuf, 0, sizeof(scanbuf));
        recvLen = 0;
        emit recvScanInfo();
    }
#endif
}

void scanSerialTalk::onTimerOut()
{
    myTimer->stop();
#ifdef SCAN_DEBUG
    QString recvData = QByteArray::fromRawData(scanbuf, recvLen-2);
    qDebug(qPrintable(recvData));
#endif

    char tempdata[strlen("ebusdr://")] = {0};
    memcpy(tempdata, scanbuf, strlen("ebusdr://"));
    if(memcmp(tempdata, "ebusdr://", strlen("ebusdr://")) == 0)
    {
        qDebug("kira --- emit driveSignRecordInfo()...");
        memcpy(&driveSignRecordInfo.signInfo, scanbuf, recvLen-2);
        driveSignRecordInfo.signInfoLen = recvLen - 2;
        emit recvDriveSignInfo();
    }
    else
    {
        printf("kira --- emit recvScanInfo()...%d\n",recvLen);
#if 0
        if(recvLen < MAX_WEIXIN_DATALEN+3)
        {
            memcpy(weixinScanInfo.scanData, scanbuf, recvLen-2);
            weixinScanInfo.scanLen = recvLen - 2;
            emit recvWeixinScanInfo();
        }
        else
#endif
        {
#if 1
            memcpy(alipayScanInfo.scanData, scanbuf, recvLen-2);
            alipayScanInfo.scanLen = recvLen - 2;
            emit recvScanInfo();
#endif
        }
    }
    memset(scanbuf, 0, sizeof(scanbuf));
    recvLen = 0;
}

void scanSerialTalk::getScanInfo(scanTradeInfo *pTradeInfo)
{
    memcpy(pTradeInfo, &tradeInfo, sizeof(scanTradeInfo));
#ifdef SCAN_DEBUG
    printf("kira --- pTradeInfo.accountState = %.2x \n", pTradeInfo->accountState);
    for(int j=0; j<sizeof(pTradeInfo->monthBalance); j++)
        printf("%.2x ", pTradeInfo->monthBalance[j]);
    printf("\n");
#endif
}

void scanSerialTalk::tradeGledCtrl(int flag)
{
    if(ledGfd > 0)
    {
        if(flag)
            write(ledGfd, "1", 1);
        else
            write(ledGfd, "0", 1);
    }
}

void scanSerialTalk::tradeRledCtrl(int flag)
{
    if(ledRfd > 0)
    {
        if(flag)
            write(ledRfd, "1", 1);
        else
            write(ledRfd, "0", 1);
    }
}

void scanSerialTalk::tradeScanCtrl(int flag)
{
    if(scanctlfd > 0)
    {
        if(flag)
            write(scanctlfd, "1", 1);
        else
            write(scanctlfd, "0", 1);
    }
}

#if 1
int scanSerialTalk::dsaVerify(unsigned char* signStr, int signStrlen, unsigned char* inputStr, int inputStrlen)
{
    DSA *dsa;
    FILE *file;

    if ((file = fopen(PUBLICKEY, "r")) == NULL)
    {
        qDebug("kira --- open pub key file error");
        return 0;
    }

    if ((dsa = PEM_read_DSA_PUBKEY(file, NULL, NULL, NULL)) == NULL)
    {
        qDebug("kira --- read pub key file error");
        ERR_print_errors_fp(stdout);
        return 0;
    }
    int is_valid_signature =
            DSA_verify(NID_md5, inputStr, inputStrlen, signStr, signStrlen, dsa);
    //qDebug("kira --- dsa_verify sig_len = %d", sig_len);
    return is_valid_signature;
}
#endif

unsigned char scanSerialTalk::AscToHex(unsigned char aChar)
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

void scanSerialTalk::getWeixinScanInfo(weixinScanInfo_t *pWeixinScanInfo)
{
    memcpy(pWeixinScanInfo->scanData, weixinScanInfo.scanData, sizeof(weixinScanInfo.scanData));
    pWeixinScanInfo->scanLen = weixinScanInfo.scanLen;
}

// Alipay
void scanSerialTalk::getAlipayScanInfo(alipayScanInfo_t *pAlipayScanInfo)
{
    memcpy(pAlipayScanInfo->scanData, alipayScanInfo.scanData, sizeof(alipayScanInfo.scanData));
    pAlipayScanInfo->scanLen = alipayScanInfo.scanLen;
}

// drive sign
void scanSerialTalk::getDriveSignRecordInfo(driveSignRecordInfo_t *pSignInfo)
{
    memcpy(&pSignInfo->signInfo, &driveSignRecordInfo.signInfo, sizeof(driveSignScanInfo_t));
    pSignInfo->signInfoLen = driveSignRecordInfo.signInfoLen;
}
