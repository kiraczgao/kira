#include "networktalk.h"
#include <QTimer>

NetworkTalk::NetworkTalk()
{
    qDebug("kira --- Initial network");

    recvLen = 0;
    gprsDatalen = 0;
    gprsCmdlen = 0;
    memset(gprsbuf, 0, sizeof(gprsbuf));

    m_stopFlag = false;
    gprsflag = false;
    gprsLedFlag = true;
    gprsPowerFlag = true;

    //新建一个QTimer对象
    beatTimer = new QTimer();
    //设置定时器每个多少毫秒发送一个timeout()信号
    beatTimer->setInterval(30000);
    //暂停定时器
    beatTimer->stop();
    connect(beatTimer, SIGNAL(timeout()), this, SLOT(beatTimerOut()));

    //新建一个QTimer对象
    gprsLedTimer = new QTimer();
    //设置定时器每个多少毫秒发送一个timeout()信号
    gprsLedTimer->setInterval(2000);
    //暂停定时器
    gprsLedTimer->stop();
    connect(gprsLedTimer, SIGNAL(timeout()), this, SLOT(gprsLedTimerOut()));

    powerfd = open("/sys/class/gpio/gpio50/value", O_RDWR);
    if(powerfd < 0)
    {
        qDebug("kira --- open gprs power failed: %d", powerfd);
    }

    powerctrlfd = open("/sys/class/gpio/gpio49/value", O_RDWR);
    if(powerctrlfd < 0)
    {
        qDebug("kira --- open gprs power control failed: %d", powerctrlfd);
    }

    gprsLedfd = open("/sys/class/gpio/gpio111/value", O_RDWR);
    if(gprsLedfd < 0)
    {
        qDebug("kira --- open gprs LED control failed: %d", powerctrlfd);
    }

    rstfd = open("/sys/class/gpio/gpio103/value", O_RDWR);
    if(rstfd < 0)
    {
        qDebug("kira --- open gprs rst failed: %d", rstfd);
    }
}

void NetworkTalk::beatTimerOut()
{
    qDebug("kira --- send beatInfo...");
    emit sendBeatInfo();
}

#if 0
int NetworkTalk::socketConnect()
{
    int temp;
    int ret;
    struct sockaddr_in server;
    socklen_t len;

    temp = socket(AF_INET, SOCK_STREAM, 0);
    if(temp == -1)
        return -1;

#if 1
    struct timeval connectTime;
    len = sizeof(connectTime);
    connectTime.tv_sec = 5;
    connectTime.tv_usec = 0;
    setsockopt(temp, SOL_SOCKET, SO_SNDTIMEO, &connectTime, len);
#endif

    memset((char *)&server, 0, sizeof(server));
    server.sin_addr.s_addr = inet_addr(serverIP.toLatin1().data());
    server.sin_family = AF_INET;
    server.sin_port = htons(serverPort.toInt());
    printf("kira --- connect ip: %s  port: %d \n", serverIP.toLatin1().data(), serverPort.toInt());
    ret = connect(temp, (struct sockaddr *)&server, sizeof(server));
    printf("kira --- connect ret = %d  errno = %d \n", ret, errno);
    if(ret == -1)
    {
        close(temp);
        return -1;
    }
    fcntl(temp, F_SETFL, O_NONBLOCK);

    return temp;
}
#endif

int NetworkTalk::socketSend(unsigned char *data, int len, int *hadsend)
{
    int total = 0;
    int ret;
    int trysend = 0;
    int nbytes;
    printf("kira --- network send data\n");
    while(total < len)
    {
again:
        nbytes = send(socketfd, data+total, len-total, 0);
        if(nbytes < 0)  //error close socket
        {
            ret = errno;
            if(ret == 11)
            {
                nbytes = 0;
                printf("kira --- error no %d \n", ret);
                goto again;
            }
            return 2;
        }
        else
        {
            if(nbytes == 0)
            {
                usleep(5);
                if(trysend++ > 20)
                {
                    break;
                }   //增加防止断网等不正常现象
            }
            else
            {
                trysend = 0;
            }
            total += nbytes;
        }
        ///////////////////////////////
    }
    *hadsend = total;
    printf("kira --- hadsend = %d, len = %d \n", *hadsend, len);
    if(*hadsend == len)
    {
        return 0;
    }
    return 1;
}

void NetworkTalk::gprsLedTimerOut()
{
    if(gprsLedFlag)
    {
        gprsLedCtrl(1);
        gprsLedFlag = false;
    }
    else
    {
        gprsLedCtrl(0);
        gprsLedFlag = true;
    }
}

void NetworkTalk::stop()
{
    QMutexLocker locker(&threadMutex);
    m_stopFlag = true;
}

int NetworkTalk :: protocol01_get_len(unsigned char *data, int datalen)
{
    short len;
    if(datalen < 7)
        return -1;
    if(data[0]!=0x55 || data[1]!=0x7a)
        return -2;
    memcpy(&len, &data[3], 2);
    printf("kira --- protocol01: len = %.2x, sizeof(short) = %.2x \n", len, sizeof(short));
    len += 7;
    return len;
}

int NetworkTalk :: protocol01_decode(unsigned char *data, int datalen, unsigned char *cmd, unsigned char **realdata, int *realdatalen)
{
    int i;
    int total = 0;
    short num;
    unsigned char check = 0;
    if(datalen <= 0)
        return 1;
    if(data[0]!=0x55 || data[1]!=0x7a)
        return 2;
    *cmd = data[2];
    memcpy(&num, &data[3], 2);
    total = 7 + num;
    if(total < datalen)
        return 3;
    *realdata = &data[6];
    for(i=0; i<total-1;i++)
    {
        check ^= data[i];
    }
    printf("kira --- protocal01: check = %.2x \n", check);
    if(data[total-1] != check)
        return 4;
    *realdatalen = num;
    return 0;
}

int NetworkTalk :: analysecmd01(int cmd, unsigned char *realdata, int datalen)
{

    switch(cmd)
    {
        case 0x01:  // 心跳回应
            qDebug("kira ---process beat ack...");
            fillBeatAckInfo((char*)realdata, datalen);
            emit recvBeatAck_v1();
            break;
        case 0x02:  // 消费回应
            qDebug("kira ---process consume ack...");
            fillConsumeAckInfo((char*)realdata, datalen);
            emit recvConsumeAck_v1();
            break;
        case 0x03:  // 账号查询

            break;
        case 0x04:  // 密钥请求回应

            break;
        case 0x05:  // 费率设置

            break;
        case 0x06:  // 线路设置

            break;
        case 0x07:  // 设备设置

            break;
        case 0x15:  // 费率查询

            break;
        case 0x16:  // 线路查询

            break;
        case 0x17:  // 设备查询

            break;
        case 0x21:  // 微信在线支付心跳回应
            qDebug("kira ---process weixin beat ack...");
            fillweixinBeatAckInfo((char*)realdata, datalen);
            emit recvWeixinBeatAck_v1();
            break;
        case 0x22:  // 微信在线支付结果
            qDebug("kira ---process consume ack...");
            //fillweixinTradeAckInfo(realdata, recvDatalen);
            fillOnlineTradeAckInfo((char*)realdata, datalen);
            emit recvWeixinTradeAck_v1();
            break;
        case 0x23:  // Alipay key请求回应
            qDebug("kira --- process Alipay keyAck...");
            fillAlipayKeyInfo((char*)realdata, datalen);
            emit recvAlipayKeyInfo_v1();
            break;
        case 0x24: // Alipay交易记录上传回应
            qDebug("kira --- process Alipay tradeAck...");
            fillAlipayTradeAckInfo((char*)realdata, datalen);
            emit recvAlipayTradeAck_v1();
            break;
        case 0x25: // Alipay黑名单请求回应
            qDebug("kira --- process Alipay blackAck...");
            fillAlipayBlackInfo((char*)realdata, datalen);
            emit recvAlipayBlackInfo_v1();
            break;
        case 0x29: // 司机签到信息回应
            qDebug("kira --- process driver sign recordAck...");
            fillDriverSignInfo((char*)realdata, datalen);
            emit recvDriverSignInfo_v1();
            break;
        case 0x28:  // 升级FTP服务器信息
            qDebug("kira --- process ftp info...");
            fillFtpInfo((char*)realdata, datalen);
            emit recvFtpInfo();
            break;
        case 0x31: //设备基本参数设置
            qDebug("kira --- process pos param set,,,");
            fillPosParamInfo((char*)realdata, datalen);
            emit recvPosParamsetInfo();
            break;
        case 0x35:  // 升级信息应答
            qDebug("kira --- process updata info ack,,,");
            //updataInfpAck_t l_updataInfpAck;
#if 1
            for(int i=0; i<datalen; i++)
                printf("%.2x ", realdata[i]);
            printf("\n");
#endif
            fillUpdataAckInfo((char*)realdata, datalen);
            emit recvUpdataInfoAck();
            break;
		case 0x36:  // 银联交易记录回应
            {
                qDebug("qwy --- process unionpay recordAck...");
                unionPayAck_t tag;
                memset(&tag,0,sizeof(unionPayAck_t));
                memcpy(&tag, (char*)realdata, datalen);
                emit recvUnionpayAck_v1(tag);
            }
            break;
        case 0x37:  // 银联异常交易记录回应
            {
                qDebug("qwy --- process unionpay abnormal recordAck...");
                unionPayAck_t tag;
                memset(&tag,0,sizeof(unionPayAck_t));
                memcpy(&tag, (char*)realdata, datalen);
                emit recvUnionpayAck_v1(tag);
            }
			break;
        case 0x38: // AlipayV2交易记录上传回应
        case 0x39:
            qDebug("kira --- process AlipayV2 tradeAck...");
            fillAlipayTradeAckInfo((char*)realdata, datalen);
            emit recvAlipayTradeAck_v1();
            break;
    }

    return 0;
}

void NetworkTalk::run()
{
    int ret = 0;
    unsigned char cmd = 0;
    unsigned char *realdata = NULL;
    int datalen = 0;
    fd_set  sets;
    struct timeval val;
    int headlen = 7;
    int totallen;
    int flag = 1;

    QMutexLocker locker(&threadMutex);
again:
    if(beatTimer->isActive())
        beatTimer->stop();
    while(1)
    {
        printf("kira --- try to connect server \n");
        socketfd = network.socketConnect(serverIP.toLatin1().data(), serverPort.toInt());
        printf("kira --- socket = %d \n", socketfd);
        if(socketfd > 0)
        {
            setsockopt(socketfd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag));
            break;
        }
        else
        {
            sleep(3);
            goto again;
        }
    }

    emit sendBeatInfo();
    if(false == beatTimer->isActive())
        beatTimer->start();

    while(1)
    {
        FD_ZERO(&sets);
        FD_SET(socketfd, &sets);

        val.tv_sec = 0;
        val.tv_usec = 5000;
        ret = select(socketfd+1, &sets, NULL, NULL, &val);



        if (ret == -1)
        {
            ;
        }
        else if(ret)
        {
            usleep(20);
            if(FD_ISSET(socketfd, &sets))
            {
                printf("kira --- headlen = %d\n", headlen);
                //ret = recv(sock, gprsbuf, headlen, 0);
                ret = recv(socketfd, gprsbuf, headlen, 0);
                if(ret <= 0)
                {
                    printf("kira --- can't get data\n");
                    close(socketfd);
                    socketfd = -1;
                    goto again;
                }
                if(ret != headlen)
                {
                    printf("kira --- can't get data1\n");
                    close(socketfd);
                    socketfd = -1;
                    goto again;
                }

                totallen = protocol01_get_len((unsigned char*)gprsbuf, headlen);
                printf("kira --- totallen = %d\n", totallen);
                if(totallen < 0)
                {
                    printf("kira --- can't get data2\n");
                    close(socketfd);
                    socketfd = -1;
                    goto again;
                }
                if(totallen > headlen)
                {
                    ret = recv(socketfd, gprsbuf+headlen, totallen-headlen, 0);
                    if(ret != totallen-headlen)
                    {
                        printf("kira --- can't get data3\n");
                        close(socketfd);
                        socketfd = -1;
                        goto again;
                    }
                }

                ret = protocol01_decode((unsigned char*)gprsbuf, totallen, &cmd, &realdata, &datalen);
                printf("kira --- protocol01_decode cmd = %x, ret = %x\n", cmd, ret);

                if(ret == 0)
                {
                    printf("kira --- check the cmd is in analysecmd01 fuction.... \n");
                    ret = analysecmd01(cmd, realdata, datalen);
                    printf("kira --- analysecmd01 ret: %d \n", ret);
                }
            }
        }

        if(m_stopFlag)
            break;
    }
    m_stopFlag = false;
}

void NetworkTalk::fillBeatAckInfo(char *pdata, int dataLen)
{
    QMutexLocker locker(&beatMutex);

    memset(&beatAckInfo, 0, sizeof(beatAckInfo_t));
    memcpy(&beatAckInfo, pdata, dataLen);
}

void NetworkTalk::readBeatAckInfo(beatAckInfo_t *pbeatAckInfo)
{
    QMutexLocker locker(&beatMutex);

    memcpy(pbeatAckInfo, &beatAckInfo, sizeof(beatAckInfo_t));
}

void NetworkTalk::fillConsumeAckInfo(char *pdata, int dataLen)
{
    QMutexLocker locker(&consumeMutex);

    memset(&consumeAckInfo, 0, sizeof(consumeAckInfo_t));
    memcpy(&consumeAckInfo, pdata, dataLen);
}

void NetworkTalk::readConsumeAckInfo(consumeAckInfo_t *pconsumeAckInfo)
{
    QMutexLocker locker(&consumeMutex);

    memcpy(pconsumeAckInfo, &consumeAckInfo, sizeof(consumeAckInfo_t));
}

// 微信在线支付
void NetworkTalk::fillweixinTradeAckInfo(char *pdata, int dataLen)
{
    QMutexLocker locker(&consumeMutex);
    qDebug("kira --- pdata： %x", pdata[0]);
    memset(&weixinTradeAck, 0, sizeof(weixinTradeAck_t));
    memcpy(&weixinTradeAck, pdata, dataLen);
}

// up
void NetworkTalk::fillUpdataAckInfo(char *pdata, int dataLen)
{
    QMutexLocker locker(&consumeMutex);
    memset(&updataInfpAck, 0, sizeof(updataInfpAck_t));
    memcpy(&updataInfpAck, pdata, dataLen);
}

void NetworkTalk::readweixinTradeAckInfo(weixinTradeAck_t* pweixinTradeAck)
{
    QMutexLocker locker(&consumeMutex);

    memcpy(pweixinTradeAck, &weixinTradeAck, sizeof(weixinTradeAck_t));
}

void NetworkTalk::fillOnlineTradeAckInfo(char *pdata, int dataLen)
{
    QMutexLocker locker(&consumeMutex);
    qDebug("kira --- online pdata： %x", pdata[4]);
    memset(&onlineTradeAck, 0, sizeof(onlineTradeAck_t));
    memcpy(&onlineTradeAck, pdata, dataLen);
}

void NetworkTalk::readOnlineTradeAckInfo(onlineTradeAck_t* ponlineTradeAck)
{
    QMutexLocker locker(&consumeMutex);

    memcpy(ponlineTradeAck, &onlineTradeAck, sizeof(onlineTradeAck_t));
}

void NetworkTalk::fillweixinBeatAckInfo(char *pdata, int dataLen)
{
    QMutexLocker locker(&beatMutex);

    memset(&weixinBeatAck, 0, sizeof(weixinBeatAck_t));
    memcpy(&weixinBeatAck, pdata, dataLen);
}

void NetworkTalk::readweixinBeatAckInfo(weixinBeatAck_t* pweixinBeatAck)
{
    QMutexLocker locker(&beatMutex);

    memcpy(pweixinBeatAck, &weixinBeatAck, sizeof(weixinBeatAck_t));
}

// up
void NetworkTalk::readUpdataAckInfo(updataInfpAck_t *pupdataInfpAck)
{
    QMutexLocker locker(&beatMutex);

    memcpy(pupdataInfpAck, &updataInfpAck, sizeof(updataInfpAck_t));
}

void NetworkTalk::gprsLedCtrl(int flag)
{
    if(gprsLedfd > 0)
    {
        if(flag)
            write(gprsLedfd, "1", 1);
        else
            write(gprsLedfd, "0", 1);
    }
}

void NetworkTalk::powerCtrl(int flag)
{
    if(powerfd > 0)
    {
        if(flag)
            write(powerfd, "1", 1);
        else
            write(powerfd, "0", 1);
    }
}

void NetworkTalk::powerctrlCtrl(int flag)
{
    if(powerctrlfd > 0)
    {
        if(flag)
            write(powerctrlfd, "1", 1);
        else
            write(powerctrlfd, "0", 1);
    }
}

void NetworkTalk::rstCtrl(int flag)
{
    if(rstfd > 0)
    {
        if(flag)
            write(rstfd, "1", 1);
        else
            write(rstfd, "0", 1);
    }
}

char NetworkTalk::AscToHex(char aChar)
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

// Alipay
void NetworkTalk::fillAlipayKeyInfo(char *pdata, int dataLen)
{
    QMutexLocker locker(&alipaykeyMutex);

    memset(&alipayKeyInfo, 0, sizeof(alipayKeyInfo_t));
    memcpy(&alipayKeyInfo, pdata, dataLen);
}

void NetworkTalk::readAlipayKeyInfo(alipayKeyInfo_t* palipayKeyInfo)
{
    QMutexLocker locker(&alipaykeyMutex);

    memcpy(palipayKeyInfo, &alipayKeyInfo, sizeof(alipayKeyInfo_t));
}

void NetworkTalk::fillAlipayBlackInfo(char *pdata, int dataLen)
{
    QMutexLocker locker(&alipayblackMutex);

    memset(&alipayBlackInfo, 0, sizeof(alipayBlackInfo_t));
    memcpy(&alipayBlackInfo, pdata, dataLen);
}

void NetworkTalk::readAlipayBlackInfo(alipayBlackInfo_t* palipayBlackInfo)
{
    QMutexLocker locker(&alipayblackMutex);

    memcpy(palipayBlackInfo, &alipayBlackInfo, sizeof(alipayBlackInfo_t));
}

void NetworkTalk::fillAlipayTradeAckInfo(char *pdata, int dataLen)
{
    QMutexLocker locker(&alipaytradeMutex);

    memset(&alipayTradeInfoAck, 0, sizeof(alipayTradeInfoAck_t));
    memcpy(&alipayTradeInfoAck, pdata, dataLen);
}

void NetworkTalk::readAlipayTradeAckInfo(alipayTradeInfoAck_t *palipayTradeInfoAck)
{
    QMutexLocker locker(&alipaytradeMutex);

    memcpy(palipayTradeInfoAck, &alipayTradeInfoAck, sizeof(alipayTradeInfoAck_t));
}

void NetworkTalk::fillDriverSignInfo(char *pdata, int dataLen)
{
    QMutexLocker locker(&driverSignMutex);

    memset(&driveSignNetAck, 0, sizeof(driveSignNetAck_t));
    memcpy(&driveSignNetAck, pdata, dataLen);
}

void NetworkTalk::readDriverSignInfo(driveSignNetAck_t *pdriveSignNetAck)
{
    QMutexLocker locker(&driverSignMutex);

    memcpy(pdriveSignNetAck, &driveSignNetAck, sizeof(driveSignNetAck_t));
}

// pos param
void NetworkTalk::fillPosParamInfo(char *pdata, int dataLen)
{
    QMutexLocker locker(&driverSignMutex);

    memset(&posParam, 0, sizeof(posParam_t));
    memcpy(&posParam, pdata, dataLen);
}

void NetworkTalk::readPosParamInfo(posParam_t *pposParam)
{
    QMutexLocker locker(&driverSignMutex);

    memcpy(pposParam, &posParam, sizeof(posParam_t));
}

// ftp info
void NetworkTalk::fillFtpInfo(char *pdata, int dataLen)
{
    QMutexLocker locker(&driverSignMutex);
#if 1
    printf("kira --- ftpInfo: ");
    for(int i=0; i<dataLen; i++)
        printf("%c", pdata[i]);
    printf("\n");
#endif
    memset(&posUpdataInfo, 0, sizeof(posUpdataInfo_t));
    memcpy(posUpdataInfo.updataInfo, pdata, dataLen);
    posUpdataInfo.infoLen = (char)dataLen;
}

void NetworkTalk::readFtpInfo(posUpdataInfo_t *pposUpdataInfo)
{
    QMutexLocker locker(&driverSignMutex);

    memcpy(pposUpdataInfo, &posUpdataInfo, sizeof(posUpdataInfo_t));
}
