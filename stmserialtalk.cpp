#include "stmserialtalk.h"
#include <QDebug>
#include <QDateTime>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include "appUpdater.h"
#ifndef Q_MOC_RUN
#include "../boost_1_58_0/boost_include.h"
#endif

//stmSerialTalk::stmSerialTalk(QWidget *parent) : QWidget(parent)
stmSerialTalk::stmSerialTalk() : stmUpdater(new stmUpdaterThread(&stmfd))
{
    //��ʼ���������ݺ���ָ��ָ��Ӧ��̬������
    m_pReadDataFunc = &stmSerialTalk::readData;
    qDebug("kira --- Initial stm32 talk");
    stmSerialData.devName = "ttySP2";
    stmSerialData.baudRate = "B115200";//"B57600";
    stmSerialData.dataBit = "8";
    stmSerialData.parity = "none";
    stmSerialData.stopBit = "1";
    recvLen = 0;
    m_stopFlag = false;
    memset(stmbuf, 0, sizeof(stmbuf));
    this->openSerial();

    //��ʼ����Ƭ��������ʱ��
    stmUpdateTimer.setInterval(MAX_STM_UPDATE_TIME);
    connect(&stmUpdateTimer, SIGNAL(timeout()), this, SLOT(processStmUpdateFail()));
    connect(stmUpdater, SIGNAL(stmUpdateFail()), this, SLOT(processStmUpdateFail()));
}

bool stmSerialTalk::openSerial()
{
    struct termios setting;
    stmfd = open(qPrintable("/dev/" + stmSerialData.devName), O_RDWR);
    if (stmfd == -1) {
        //QMessageBox::warning(this, "stm open error", strerror(errno));
        goto openerr;
    }
    tcgetattr(stmfd, &setting);
    //  ���ò�����
    speed_t speed;
    if (stmSerialData.baudRate == "B4800") {
        speed = B4800;
    } else if (stmSerialData.baudRate == "B9600") {
        speed = B9600;
    } else if (stmSerialData.baudRate == "B19200") {
        speed = B19200;
    } else if (stmSerialData.baudRate == "B38400") {
        speed = B38400;
    } else if (stmSerialData.baudRate == "B57600") {
        speed = B57600;
    } else if (stmSerialData.baudRate == "B115200") {
        speed = B115200;
    } else if (stmSerialData.baudRate == "B230400") {
        speed = B230400;
    } else if (stmSerialData.baudRate == "B460800") {
        speed = B460800;
    } else {
        //QMessageBox::warning(this, "error", "stm speed error!");
        goto seterr;
    }
    cfsetispeed(&setting, speed);
    cfsetospeed(&setting, speed);
    cfmakeraw(&setting);
    //  ��������λ
    if (stmSerialData.dataBit == "8") {
        setting .c_cflag |= CS8;
    } else if (stmSerialData.dataBit == "7") {
        setting.c_cflag |= CS7;
    } else if (stmSerialData.dataBit == "6") {
        setting.c_cflag |= CS6;
    } else if (stmSerialData.dataBit == "5") {
        setting.c_cflag |= CS5;
    } else {
        //QMessageBox::warning(this, "error", "stm dataBit error!");
        goto seterr;
    }
    //  ����parity
    if (stmSerialData.parity == "none") {
        setting.c_cflag &= ~PARENB;
        setting.c_iflag &= ~INPCK;
    } else if (stmSerialData.parity == "odd") {
        setting.c_cflag |= (PARODD | PARENB);
        setting.c_iflag |= INPCK;
    } else if (stmSerialData.parity == "even") {
        setting.c_cflag |= PARENB;
        setting.c_cflag &= ~PARODD;
        setting.c_iflag |= INPCK;
    } else {
        //QMessageBox::warning(this, "error", "stm dataBit error!");
        goto seterr;
    }
    //  ����ֹͣλ
    if (stmSerialData.stopBit == "1") {
        setting.c_cflag &= ~CSTOPB;
    } else if (stmSerialData.stopBit == "2") {
        setting.c_cflag |= CSTOPB;
    } else {
        //QMessageBox::warning(this, "error", "stm stopBit error!");
        goto seterr;
    }

    tcflush(stmfd, TCIFLUSH);
    setting.c_cc[VTIME] = 0;
    setting.c_cc[VMIN] = 0;
    tcsetattr(stmfd, TCSANOW, &setting);

#if 0
    stmMonitor = new QSocketNotifier(stmfd, QSocketNotifier::Read, NULL);
    connect(stmMonitor, SIGNAL(activated(int)), this, SLOT(readData()));
    stmMonitor->setEnabled(true);
#endif

    return true;
seterr:
    ::close(stmfd);
openerr:
    return false;
}
static unsigned char bcd2ascii[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
static unsigned char ascii2bcd1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
static unsigned char ascii2bcd2[6]  = {0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
void stmSerialTalk::BCD2ASCII(unsigned char* asc,const unsigned char* bcd,int len)
{
    unsigned char c = 0;
    int i;
    for(i = 0; i < len; i++) {
        //first BCD
        c = *bcd >> 4;
        *asc++ = bcd2ascii[c];
        //second
        c = *bcd & 0x0f;
        *asc++ = bcd2ascii[c];
        bcd++;
    }
}
void stmSerialTalk::ASCII2BCD(unsigned char *bcd, const unsigned char *asc, int len)
{
    unsigned char c = 0;
    int index = 0;
    int i = 0;
    len >>= 1;
    for(; i < len; i++) {
        //first BCD
        if(*asc >= 'A' && *asc <= 'F') {
            index = *asc - 'A';
            c  = ascii2bcd2[index] << 4;
        } else if(*asc >= '0' && *asc <= '9') {
            index = *asc - '0';
            c  = ascii2bcd1[index] << 4;
        }
        asc++;
        //second BCD
        if(*asc >= 'A' && *asc <= 'F') {
            index = *asc - 'A';
            c  |= ascii2bcd2[index];
        } else if(*asc >= '0' && *asc <= '9') {
            index = *asc - '0';
            c  |= ascii2bcd1[index];
        }
        asc++;
        *bcd++ = c;
    }
}

void stmSerialTalk::readData()
{
    //recvLen = read(stmfd, stmbuf, sizeof(stmbuf));
    //qDebug("kira --- receive stmInfo over... len=%d", recvLen);
    //qDebug(stmbuf);
    char cmd;
    int datalen;
    recvLen = read(stmfd, stmbuf, headlen);
#if 0
    qDebug("kira --- receive stmInfo head over...");
    for(int i=0; i<headlen; i++)
        qDebug("stmfd: %.2x", stmbuf[i]);
#endif
    if(0 == checkHead((char*)stmbuf, &datalen, &cmd))
    {
     //   usleep(50*1000);
        recvLen = read(stmfd, &stmbuf[headlen], datalen+1);
#if 0
        for(int i=0; i<datalen+7; i++)
            printf("%.2x ", stmbuf[i]);
        printf("\n");
#endif
        if(0 == checkVertify((char*)stmbuf, datalen+7))
        {
            switch(cmd)
            {
                case 0x01:
                    processPress((char*)&stmbuf[6], datalen);
                    break;
                case 0xA2:
                    processUnionPayProcA2();
                    processCardTradeInfo((char*)&stmbuf[6], datalen);
                    break;
                case 0xA3:
                    processUnionPayTermMkInfo((char*)&stmbuf[6], datalen);
                    break;
                case 0x21:  //��Ƭ������������Ӧָ��
                    if(!stmUpdater)
                    {
                        //�����̲߳���ȷ
                        qDebug("Stm updater is incorrect, update failed!!");
                    }
                    else
                    {
                        //���������л���boot̬
                        m_pReadDataFunc = &stmSerialTalk::readDataBoot;
                        //���������߳�
                        stmUpdater->start();
                        //������ʱ��
                        stmUpdateTimer.start();
                    }
                    break;
                case 0xB1:
                    printf("pscam ret ack: ");
                    for(int i=0; i<datalen; i++)
                        printf("%.2x ",stmbuf[i+6]);
                    printf("\n");
                    emit recvPscamAck(stmbuf[6]);
                    break;
                case 0xBF:
                    printf("qwy---BF\n");
                    msleep(10);
                   // char cdata = 0x00;
                    stmSendData(0xBF,(char*)&stmbuf[6],datalen);
                    printf("qwy---send BF\n");
                    break;
                case 0xC1:
                    {
                        posParam_t tag;
                        memset(&tag,0,sizeof(posParam_t));
                        //��·��
                        BCD2ASCII((unsigned char*)tag.buslineID,(unsigned char*)&stmbuf[9],4);
                        //�������
                        BCD2ASCII((unsigned char*)tag.busID,(unsigned char*)&stmbuf[13],3);
                        //Ʊ��
                        memcpy(tag.tickets,&stmbuf[16],2);
                        emit setPosParam_v2(tag);
                        //��Ӧ
                        char cData = 0x01;
                        stmSendData(0xC1,&cData,1);
                    }
                    break;
                case 0xC2:
                    {
                        //�������(3 BCD)
                        //������(1 BCD)
                        unsigned char cardType = (unsigned char)stmbuf[9];
                        //����Ա����(4 BCD)
                        char cDriver[9] = {0};
                        BCD2ASCII((unsigned char*)cDriver,(unsigned char*)&stmbuf[10],4);
                        QString strDriverId = cDriver;
                        emit driverSign_v2(strDriverId,cardType);
                    }
                    break;
            }
        }
    }

    memset(stmbuf, 0, sizeof(stmbuf));
    recvLen = 0;
    //emit recvCardInfo();
}

void stmSerialTalk::processCardTradeInfo(char *pdata, int datalen)
{
    memcpy(&cardTradeInfoA2, pdata, datalen);

    emit recvCardTradeInfoA2();
}

void stmSerialTalk::processUnionPayTermMkInfo(char *pdata, int datalen)
{
    mytermMk = QByteArray::fromRawData(pdata, datalen).toHex();
    qDebug("kira --- mytermMk: ");
    qDebug(qPrintable(mytermMk));
    emit recvTermMkInfoA3();
}

void stmSerialTalk::getCardTradeInfo(cardreceiveA2 *pcardreceiveA2)
{
    memcpy(pcardreceiveA2, &cardTradeInfoA2, sizeof(cardreceiveA2));
}

void stmSerialTalk::stop()
{
    QMutexLocker locker(&threadMutex);
    m_stopFlag = true;
}

void stmSerialTalk::run()
{
    int ret = 0;
    fd_set  sets;
    struct timeval val;
    printf("start stmSerialTalk 115200... \n");

    while(1)
    {
        QMutexLocker locker(&threadMutex);
        FD_ZERO(&sets);
        FD_SET(stmfd, &sets);

        val.tv_sec = 0;
        val.tv_usec = 5*1000;//500*1000
        ret = select(stmfd+1, &sets, NULL, NULL, &val);
        if (ret == -1)
        {
            ;
        }
        else if(ret)
        {
         //   usleep(20);
            msleep(20);
            if(FD_ISSET(stmfd, &sets))
            {
                //readData();
                //����ָ����������ĺ���ָ��
                //��������� 1.ָ��Ӧ��̬�������� 2.ָ��boot̬��������
                (this->*m_pReadDataFunc)();
            }
        }

        if(m_stopFlag)
            break;
    }
    m_stopFlag = false;
}

int stmSerialTalk::checkHead(char *pdata, int* datalen, char* cmd)
{
    if((pdata[0] != head_f) || (pdata[1] != head_s))
    {
        qDebug("stm32 head error...");
        return 1;
    }
    *datalen = pdata[3] + pdata[4]*256;
    *cmd = pdata[2];
    return 0;
}

int stmSerialTalk::checkVertify(char *pdata, int datalen)
{
    char checksum = 0;
    for(int i=0; i<datalen-1; i++)
        checksum ^= pdata[i];
    if(checksum != pdata[datalen-1])
    {
        qDebug("stm32 vertify error... %.2x", checksum);
        return 1;
    }

    return 0;
}

void stmSerialTalk::processPress(char *pdata, int datalen)
{
    qDebug("key press...");
    for(int i=0; i<datalen; i++)
        printf("%.2x ", pdata[i]);
    printf("\n");
    switch(pdata[0])
    {
        case 0x70:
            emit recvShortpress();
            break;
        case 0x71:
            emit recvLongpress();
            break;
        case 0x72:
            emit recvLeftpress();
            break;
        case 0x73:
            emit recvRightpress();
            break;
        case 0x80://show pos param
            emit recvShowPosParam();
            break;
        default:
            qDebug("kira --- stm32 key wrong value...");
            break;
    }
}

void stmSerialTalk::writeData(char* data, int datalen)
{
    int wlen = write(stmfd, data, datalen);
    qDebug("kira --- stm write data datalen=%d, wlen=%d", datalen, wlen);
    //QString writeData = QByteArray::fromRawData(data, datalen).toHex();
    //qDebug(qPrintable(writeData));
#if 1
    for(int i=0; i<datalen; i++)
    {
        printf("%.2x ", data[i]);
    }
    printf("\n");
#endif
}

void stmSerialTalk::stmVoiceCmd(char type)
{
    switch(type)
    {
        case 12:
            stmVoiceCmdDebit(0x01);
            break;
        case 10:
            stmVoiceRescan(0x01);
            break;
        case 26:
            stmVoiceTrade(0x01);
            break;
        case 2:
            stmVoiceWait(0x01);
            break;
        case 19:
            stmVoiceQRError(0x01);
            break;
        case 17:
            stmVoiceBlack(0x01);
            break;
        case 27:
            stmVoiceUseBusQR(0x01);
            break;
        case 25:
            stmVoiceDriverSign(0x01);
            break;
        case 46:
        case 58:
        case 59:
        {
            char check = 0;
            char voice[20];
            int len = 1;

            voice[0] = 0x55;
            voice[1] = 0x7a;
            voice[2] = 0x01;
            memcpy(&voice[3], &len, 2);
            voice[5] = 0x00;
            voice[6] = type;
            for(int i=0; i<6+len; i++)
                check ^= voice[i];
            voice[6+len] = check;
            writeData(voice, len+7);
        }
            break;
        default:
            qDebug("kira --- stm can't find type");
    }
}

void stmSerialTalk::stmVoiceCmdDebit(char cmd)
{
    char check = 0;
    char voiceDebit[20];
    qDebug(qPrintable(debit));
    int ldebit = debit.toDouble()*100.0;
    int len = 3;

    voiceDebit[0] = 0x55;
    voiceDebit[1] = 0x7a;
    voiceDebit[2] = cmd;
    memcpy(&voiceDebit[3], &len, 2);
    voiceDebit[5] = 0x00;
    voiceDebit[6] = 12;
    memcpy(&voiceDebit[7], &ldebit, 2);
    for(int i=0; i<6+len; i++)
        check ^= voiceDebit[i];
    voiceDebit[6+len] = check;

    writeData(voiceDebit, len+7);
}

void stmSerialTalk::stmVoiceRescan(char cmd)
{
    char check = 0;
    char voiceRescan[20];
    int len = 1;

    voiceRescan[0] = 0x55;
    voiceRescan[1] = 0x7a;
    voiceRescan[2] = cmd;
    memcpy(&voiceRescan[3], &len, 2);
    voiceRescan[5] = 0x00;
    voiceRescan[6] = 10;
    for(int i=0; i<6+len; i++)
        check ^= voiceRescan[i];
    voiceRescan[6+len] = check;

    writeData(voiceRescan, len+7);
}

void stmSerialTalk::stmVoiceTrade(char cmd)
{
    char check = 0;
    char voice[20];
    int len = 1;

    voice[0] = 0x55;
    voice[1] = 0x7a;
    voice[2] = cmd;
    memcpy(&voice[3], &len, 2);
    voice[5] = 0x00;
    voice[6] = 26;
    for(int i=0; i<6+len; i++)
        check ^= voice[i];
    voice[6+len] = check;

    writeData(voice, len+7);
}

void stmSerialTalk::stmVoiceWait(char cmd)
{
    char check = 0;
    char voice[20];
    int len = 1;

    voice[0] = 0x55;
    voice[1] = 0x7a;
    voice[2] = cmd;
    memcpy(&voice[3], &len, 2);
    voice[5] = 0x00;
    voice[6] = 2;
    for(int i=0; i<6+len; i++)
        check ^= voice[i];
    voice[6+len] = check;

    writeData(voice, len+7);
}

void stmSerialTalk::stmVoiceQRError(char cmd)
{
    char check = 0;
    char voice[20];
    int len = 1;

    voice[0] = 0x55;
    voice[1] = 0x7a;
    voice[2] = cmd;
    memcpy(&voice[3], &len, 2);
    voice[5] = 0x00;
    voice[6] = 19;
    for(int i=0; i<6+len; i++)
        check ^= voice[i];
    voice[6+len] = check;

    writeData(voice, len+7);
}

void stmSerialTalk::stmVoiceBlack(char cmd)
{
    char check = 0;
    char voice[20];
    int len = 1;

    voice[0] = 0x55;
    voice[1] = 0x7a;
    voice[2] = cmd;
    memcpy(&voice[3], &len, 2);
    voice[5] = 0x00;
    voice[6] = 17;
    for(int i=0; i<6+len; i++)
        check ^= voice[i];
    voice[6+len] = check;

    writeData(voice, len+7);
}

void stmSerialTalk::stmVoiceUseBusQR(char cmd)
{
    char check = 0;
    char voice[20];
    int len = 1;

    voice[0] = 0x55;
    voice[1] = 0x7a;
    voice[2] = cmd;
    memcpy(&voice[3], &len, 2);
    voice[5] = 0x00;
    voice[6] = 27;
    for(int i=0; i<6+len; i++)
        check ^= voice[i];
    voice[6+len] = check;

    writeData(voice, len+7);
}

void stmSerialTalk::stmVoiceDriverSign(char cmd)
{
    char check = 0;
    char voice[20];
    int len = 1;

    voice[0] = 0x55;
    voice[1] = 0x7a;
    voice[2] = cmd;
    memcpy(&voice[3], &len, 2);
    voice[5] = 0x00;
    voice[6] = 25;
    for(int i=0; i<6+len; i++)
        check ^= voice[i];
    voice[6+len] = check;

    writeData(voice, len+7);
}

void stmSerialTalk::stmClearLEDDisp()
{
    char check = 0;
    char data[20];
    int len = 1;

    data[0] = 0x55;
    data[1] = 0x7a;
    data[2] = 0x02;
    memcpy(&data[3], &len, 2);
    data[5] = 0x00;
    data[6] = 0;
    for(int i=0; i<6+len; i++)
        check ^= data[i];
    data[6+len] = check;

    writeData(data, len+7);
}

void stmSerialTalk::unionPayTradeFailed()
{
    char check = 0;
    char data[20];
    int len = 3;

    data[0] = 0x55;
    data[1] = 0x7a;
    data[2] = 0x01;
    memcpy(&data[3], &len, 2);
    data[5] = 0x00;
    data[6] = 11;
    int itickets = (int)tickets.toDouble()*100;
    memcpy(&data[7], &itickets, 2);
    for(int i=0; i<6+len; i++)
        check ^= data[i];
    data[6+len] = check;

    writeData(data, len+7);
}

void stmSerialTalk::unionPayTradeSuccess()
{
    char check = 0;
    char data[20];
    int len = 3;

    data[0] = 0x55;
    data[1] = 0x7a;
    data[2] = 0x01;
    memcpy(&data[3], &len, 2);
    data[5] = 0x00;
    data[6] = 12;
    int itickets = (int)tickets.toDouble()*100;
    memcpy(&data[7], &itickets, 2);
    for(int i=0; i<6+len; i++)
        check ^= data[i];
    data[6+len] = check;

    writeData(data, len+7);
}

void stmSerialTalk::processUnionPayProcA2()
{
    char check = 0;
    char data[30];
    int len = 1;

    data[0] = 0x55;
    data[1] = 0x7a;
    data[2] = 0x01;
    memcpy(&data[3], &len, 2);
    data[5] = 0x00;
    data[6] = 98;

    for(int i=0; i<6+len; i++)
        check ^= data[i];
    data[6+len] = check;

    writeData(data, len+7);
}

void stmSerialTalk::processUnionPayProcA1()
{
    qDebug("kira --- deal processUnionPayPrecA1...");

#if 1
    QString currentDateTime = QDateTime::currentDateTime().addSecs(UTC_TIMEDIFF).toString("yyyyMMddhhmmss");
    unsigned char cdateTime[14];
    memcpy((char*)cdateTime, currentDateTime.toLatin1().data(), currentDateTime.length());
    unsigned char dateTime[7];
    for(int i=0; i<7; i++)
    {
        unsigned char dateTimeH = AscToHex(cdateTime[2*i]);
        unsigned char dateTimeL = AscToHex(cdateTime[2*i+1]);

        dateTime[i] = ((dateTimeH<<4)&0xf0) + (dateTimeL&0x0f);
    }
#endif

    char check = 0;
    char data[30];
    int len = 11;

    data[0] = 0x55;
    data[1] = 0x7a;
    data[2] = 0xA1;
    memcpy(&data[3], &len, 2);
    data[5] = 0x00;
    memcpy(&data[6], dateTime, 7);
    int itickets = (int)(tickets.toDouble()*100);
    memcpy(&data[13], &itickets, 4);
    for(int i=0; i<6+len; i++)
        check ^= data[i];
    data[6+len] = check;

    writeData(data, len+7);
}

void stmSerialTalk::stmSendData(unsigned char command, char* pdata, int len)
{
    char check = 0;
    char data[256];
    memset(data,0,256);

    data[0] = 0x55;
    data[1] = 0x7a;
    data[2] = command;
    memcpy(&data[3], &len, 2);
    data[5] = 0x00;
    if(len>0){
        memcpy(&data[6],pdata,len);
    }

    for(int i=0; i<6+len; i++)
        check ^= data[i];
    data[6+len] = check;

    writeData(data, len+7);
}

void stmSerialTalk::stmSendPsamInfo(pscamInfo_t l_pscamInfo)
{
    char check = 0;
    char data[256];
    int len = 4 + l_pscamInfo.cardNoLen + l_pscamInfo.cardDataLen;

    data[0] = 0x55;
    data[1] = 0x7a;
    data[2] = 0xB1;
    memcpy(&data[3], &len, 2);
    data[5] = 0x00;

    memcpy(&data[6], &l_pscamInfo.cardNoLen, 2);
    memcpy(&data[6+2], &l_pscamInfo.cardDataLen, 2);
    memcpy(&data[6+2+2], l_pscamInfo.cardNo, l_pscamInfo.cardNoLen);
    memcpy(&data[6+2+2+l_pscamInfo.cardNoLen], l_pscamInfo.cardData, l_pscamInfo.cardDataLen);

    for(int i=0; i<6+len; i++)
        check ^= data[i];
    data[6+len] = check;

    writeData(data, len+7);
}

unsigned char stmSerialTalk::AscToHex(unsigned char aChar)
{
    if((aChar>=0x30)&&(aChar<=0x39))
        aChar -= 0x30;
    else if((aChar>=0x41)&&(aChar<=0x46))//��д��ĸ
        aChar -= 0x37;
    else if((aChar>=0x61)&&(aChar<=0x66))//Сд��ĸ
        aChar -= 0x57;
    else
        aChar = 0xff;
    return aChar;
}

unsigned char stmSerialTalk::HexToAsc(unsigned char aHex)
{
    unsigned char  ASCII_Data;
    ASCII_Data = aHex & 0x0f;
    if(ASCII_Data < 10)
        ASCII_Data = ASCII_Data + 0x30;         //��0--9��
    else
        ASCII_Data = ASCII_Data + 0x37;         //��A--F��
    return ASCII_Data;
}

//stm boot���մ�����
void stmSerialTalk::readDataBoot()
{
    const int BOOT_DATALEN = 12;        //boot���ݳ��ȹ̶���12�ֽ�
    char *pBuf = stmbuf;
    int nReadLen = read(stmfd, pBuf, BOOT_DATALEN);
    pBuf += nReadLen;                   //����ָ��ƫ��
    //����ճ��
    int nRemainingBytes = BOOT_DATALEN - nReadLen;
    if(nRemainingBytes < 0 || nReadLen == -1)
    {
        qDebug("Bad boot datalen: %d, drop this data...\n", nReadLen);
        memset(stmbuf, 0, BOOT_DATALEN);
        return;
    }
    else if(nRemainingBytes > 0)
    {
        while(nRemainingBytes > 0)
        {
            //�ȴ�5����
            usleep(1000*5);
            nReadLen = read(stmfd, pBuf, nRemainingBytes);
            pBuf += nReadLen;
            nRemainingBytes -= nReadLen;
            if(nRemainingBytes < 0 || nReadLen == -1)
            {
                qDebug("Bad boot datalen: %d, drop this data...\n", nReadLen);
                memset(stmbuf, 0, BOOT_DATALEN);
                return;
            }
        }
    }
    //����������Ϊһ��������
    //��ӡ�յ�������
    int i = 0;  //ѭ������
    printf("Receive stm boot comdata:");
    for(i = 0; i != BOOT_DATALEN; i++)
    {
        printf(" %02X", stmbuf[i]);
    }
    printf("\n");

    //���̶�����λ�Ƿ�Ϸ�
    if ((stmbuf[0]!=0x00) || (stmbuf[1]!=0x00) || (stmbuf[2]!=0xBB) || (stmbuf[5]!=0xCC) || (stmbuf[6]!=0xAA))
    {
        qDebug("Fix data check error, drop data...\n");
        return;
    }

    //���У��λ
    char verify = 0;
    for(i = 0; i != 3; i++)
    {
        verify ^= stmbuf[7 + i];
    }
    if(verify != stmbuf[10])
    {
        qDebug("Verify check error, drop data...\n");
        return;
    }

    //��������
    switch(stmbuf[3])
    {
    //�������
    case 0x16:
        qDebug("stm restart successful, stmApp update success.\n");
        stmUpdater->m_updateStm_update_ok = true;
        m_pReadDataFunc = &stmSerialTalk::readData;
        //ֹͣ��ʱ��
        stmUpdateTimer.stop();
        //ֹͣ�����߳�
        stmUpdater->exit();
        //���͵�Ƭ�������ɹ��ź�
        emit stmUpdateSuccess();
        break;
    //ɾ��Ӧ�ó������
    case 0x17:
        qDebug("StmApp delete OK.\n");
        stmUpdater->m_updateStm_deleteOK = true;
        break;
    //�������ݽ��ջ�Ӧ
    case 0x18:
        qDebug("StmApp send data reply.\n");
    {
        if(stmbuf[9] == 0)
        {
            //��Ӧ���ճɹ�
            stmUpdater->stmUpdateCond.wakeAll();    //���ѵȴ������߳�
            stmUpdater->stmUpdateMutex.lock();
            ++stmUpdater->m_updateStm_updateAnswer;
            stmUpdater->stmUpdateMutex.unlock();
        }
        else
        {
            qDebug("StmApp 0x18 return code error: %d", stmbuf[9]);
        }
    }
        break;
    //����boot�ɹ�
    case 0x19:
        qDebug("StmApp into boot ok.\n");
        stmUpdater->m_updateStm_intoBootOK = true;
        break;
    default:
        qDebug("Unknown command: %d\n", stmbuf[3]);
        break;
    }
}

void stmSerialTalk::processStmUpdateFail()
{
    //��Ƭ������ʧ�ܴ���
    qDebug("stmUpdateFail signal catched, update failed.");
    stmUpdateTimer.stop();
    stmUpdater->exit(0);            //ǿ�н��������߳�
    m_pReadDataFunc = &stmSerialTalk::readData;    //��������ָ������ָ��Ӧ��̬
}

