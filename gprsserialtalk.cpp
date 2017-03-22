#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>

#include "gprsserialtalk.h"
#include <QTimer>

//gprsSerialTalk::gprsSerialTalk(QWidget *parent) : QWidget(parent)
gprsSerialTalk::gprsSerialTalk()
{
    qDebug("kira --- Initial gprs");
    gprsSerialData.devName = "ttySP3";
    gprsSerialData.baudRate = "B115200";
    gprsSerialData.dataBit = "8";
    gprsSerialData.parity = "none";
    gprsSerialData.stopBit = "1";
    recvLen = 0;
    gprsDatalen = 0;
    gprsCmdlen = 0;
    memset(gprsbuf, 0, sizeof(gprsbuf));
    this->openSerial();
    gprsInit = cmdATE;
    recvDataErrorTime = 0;
    recvCmdErrorTime = 0;
    initGprsTime = 0;
    serverDataErrorTime = 0;
    m_stopFlag = false;
    gprsflag = false;
    gprsLedFlag = true;
    gprsPowerFlag = true;

    //新建一个QTimer对象
    myTimer = new QTimer();
    //设置定时器每个多少毫秒发送一个timeout()信号
    myTimer->setInterval(10000);
    //暂停定时器
    myTimer->stop();
    connect(myTimer, SIGNAL(timeout()), this, SLOT(onTimerOut()));

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

bool gprsSerialTalk::openSerial()
{
    struct termios setting;
    gprsfd = open(qPrintable("/dev/" + gprsSerialData.devName), O_RDWR);
    if (gprsfd == -1) {
        //QMessageBox::warning(this, "gprs open error", strerror(errno));
        goto openerr;
    }
    tcgetattr(gprsfd, &setting);
    //  设置波特率
    speed_t speed;
    if (gprsSerialData.baudRate == "B4800") {
        speed = B4800;
    } else if (gprsSerialData.baudRate == "B9600") {
        speed = B9600;
    } else if (gprsSerialData.baudRate == "B19200") {
        speed = B19200;
    } else if (gprsSerialData.baudRate == "B38400") {
        speed = B38400;
    } else if (gprsSerialData.baudRate == "B57600") {
        speed = B57600;
    } else if (gprsSerialData.baudRate == "B115200") {
        speed = B115200;
    } else if (gprsSerialData.baudRate == "B230400") {
        speed = B230400;
    } else if (gprsSerialData.baudRate == "B460800") {
        speed = B460800;
    } else {
        //QMessageBox::warning(this, "error", "gprs speed error!");
        goto seterr;
    }
    cfsetispeed(&setting, speed);
    cfsetospeed(&setting, speed);
    cfmakeraw(&setting);
    //  设置数据位
    if (gprsSerialData.dataBit == "8") {
        setting .c_cflag |= CS8;
    } else if (gprsSerialData.dataBit == "7") {
        setting.c_cflag |= CS7;
    } else if (gprsSerialData.dataBit == "6") {
        setting.c_cflag |= CS6;
    } else if (gprsSerialData.dataBit == "5") {
        setting.c_cflag |= CS5;
    } else {
        //QMessageBox::warning(this, "error", "gprs dataBit error!");
        goto seterr;
    }
    //  设置parity
    if (gprsSerialData.parity == "none") {
        setting.c_cflag &= ~PARENB;
        setting.c_iflag &= ~INPCK;
    } else if (gprsSerialData.parity == "odd") {
        setting.c_cflag |= (PARODD | PARENB);
        setting.c_iflag |= INPCK;
    } else if (gprsSerialData.parity == "even") {
        setting.c_cflag |= PARENB;
        setting.c_cflag &= ~PARODD;
        setting.c_iflag |= INPCK;
    } else {
        //QMessageBox::warning(this, "error", "gprs dataBit error!");
        goto seterr;
    }
    //  设置停止位
    if (gprsSerialData.stopBit == "1") {
        setting.c_cflag &= ~CSTOPB;
    } else if (gprsSerialData.stopBit == "2") {
        setting.c_cflag |= CSTOPB;
    } else {
        //QMessageBox::warning(this, "error", "gprs stopBit error!");
        goto seterr;
    }

    tcflush(gprsfd, TCIFLUSH);
    setting.c_cc[VTIME] = 0;
    setting.c_cc[VMIN] = 0;
    tcsetattr(gprsfd, TCSANOW, &setting);

#if 0
    gprsMonitor = new QSocketNotifier(gprsfd, QSocketNotifier::Read, NULL);
    connect(gprsMonitor, SIGNAL(activated(int)), this, SLOT(readData()));
    gprsMonitor->setEnabled(true);
#endif

    return true;
seterr:
    ::close(gprsfd);
openerr:
    return false;
}

void gprsSerialTalk::onTimerOut()
{
    //sendGprsData((char*)"kira", 4);
    InitialGprs();
}

void gprsSerialTalk::beatTimerOut()
{
    if(finishINIT == gprsInit)
    {
        qDebug("kira --- send beatInfo...");
        emit sendBeatInfo();
    }
}

void gprsSerialTalk::gprsLedTimerOut()
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

void gprsSerialTalk::stop()
{
    QMutexLocker locker(&threadMutex);
    m_stopFlag = true;
}

void gprsSerialTalk::run()
{
    int ret = 0;
    fd_set  sets;
    struct timeval val;
    while(1)
    {
        QMutexLocker locker(&threadMutex);
        FD_ZERO(&sets);
        FD_SET(gprsfd, &sets);

        val.tv_sec = 0;
        val.tv_usec = 5000;
        ret = select(gprsfd+1, &sets, NULL, NULL, &val);
        if (ret == -1)
        {
            ;
        }
        else if(ret)
        {
            usleep(20);
            if(FD_ISSET(gprsfd, &sets))
            {
                readData();
            }
        }

        if(m_stopFlag)
            break;
    }
    m_stopFlag = false;
}

void gprsSerialTalk::dealGprsTalkError()
{
    if(++recvCmdErrorTime > 5)
    {
        recvCmdErrorTime = 0;
        gprsLedTimer->stop();
        InitialGprs();
    }
}

void gprsSerialTalk::readData()
{
    int dataLenL;
    int dataLenH;
    int headlen;
    recvLen = read(gprsfd, gprsbuf, sizeof(gprsbuf));
    qDebug("kira --- receive gprsInfo over... len=%d", recvLen);
    QString recvData = QByteArray::fromRawData(gprsbuf, recvLen);
    qDebug(qPrintable(recvData));
    myTimer->stop();

#if 1
    if(gprsInit != finishINIT)
    {
        memcpy(&gprsCmd[gprsCmdlen], gprsbuf, recvLen);
        gprsCmdlen += recvLen;
        if(gprsCmdlen >= RECVDATA_LEN)
        {
            memset(gprsCmd, 0, sizeof(gprsCmd));
            gprsCmdlen = 0;
        }

        if(gprsCmdlen > 2)
        {
            if((gprsCmd[gprsCmdlen-2] != 0x0d) && (gprsCmd[gprsCmdlen-1] != 0x0a))
                return;
        }
        else
            return;
    }
#endif

    switch(gprsInit)
    {
        case cmdATE:
            qDebug("kira --- gprs initial ATE result");
            if(strstr(gprsCmd, "OK"))
            {
                gprsInit = cmdCPIN;
                if(gprsPowerFlag)
                {
                    gprsPowerFlag = false;
                    powerCtrl(0);
                }
            }

            sleep(1);
            writeGprsCmd(gprsInit);
            break;
        case cmdCPIN:      
            qDebug("kira --- gprs initial CPIN result");
            if(strstr(gprsCmd, "OK"))
            {
                //gprsInit = cmdCGREG;
                gprsInit = cmdCNMI;
                sleep(1);
                writeGprsCmd(gprsInit);
            }
            else if(strstr(gprsCmd, "ERROR"))
            {
                dealGprsTalkError();
                gprsInit = cmdATE;
                sleep(1);
                writeGprsCmd(gprsInit);
            }
            break;
#if 1
        case cmdCGREG:
            qDebug("kira --- gprs initial CGREG result");
            if(strstr(gprsCmd, "OK"))
            {
                gprsInit = cmdCNMI;
                sleep(1);
                writeGprsCmd(gprsInit);
            }
            else if(strstr(gprsCmd, "ERROR"))
            {
                dealGprsTalkError();
                sleep(2);
                writeGprsCmd(gprsInit);
            }
            break;
#endif
        case cmdCNMI:
            qDebug("kira --- gprs initial CNMI result");
            if(strstr(gprsCmd, "OK"))
            {
                gprsInit = cmdCMGF;
                sleep(2);
                writeGprsCmd(gprsInit);
            }
            else if(strstr(gprsCmd, "ERROR"))
            {
                dealGprsTalkError();
                sleep(3);
                writeGprsCmd(gprsInit);
            }
            break;
        case cmdCMGF:
            qDebug("kira --- gprs initial CMGF result");
            if(strstr(gprsCmd, "OK"))
            {
                gprsInit = cmdCSMP;
                sleep(2);
                writeGprsCmd(gprsInit);
            }
            else if(strstr(gprsCmd, "ERROR"))
            {
                dealGprsTalkError();
                sleep(3);
                writeGprsCmd(gprsInit);
            }
            break;
        case cmdCSMP:           
            qDebug("kira --- gprs initial CSMP result");
            if(strstr(gprsCmd, "OK"))
            {
                gprsInit = queryCGREG;
                sleep(2);
                writeGprsCmd(gprsInit);
            }
            else if(strstr(gprsCmd, "ERROR"))
            {
                dealGprsTalkError();
                sleep(3);
                writeGprsCmd(gprsInit);
            }
            break;
        case queryCGREG:         
            qDebug("kira --- gprs query CGREG result");
            if(strstr(gprsCmd, "OK"))
            {
                gprsInit = cmdCPMS;
                sleep(2);
                writeGprsCmd(gprsInit);
            }
            else if(strstr(gprsCmd, "ERROR"))
            {
                dealGprsTalkError();
                sleep(3);
                writeGprsCmd(gprsInit);
            }

            break;
        case cmdCPMS:        
            qDebug("kira --- gprs initial CPMS result");
            if(strstr(gprsCmd, "OK"))
            {
                gprsInit = cmdMIPCALL;
                sleep(2);
                writeGprsCmd(gprsInit);
                recvCmdErrorTime = 0;
            }
            else if(strstr(gprsCmd, "ERROR"))
            {
                dealGprsTalkError();
                sleep(3);
                writeGprsCmd(gprsInit);
            }
            break;
        case cmdMIPDIS:           
            qDebug("kira --- gprs initial MIPDIS result");
#if 0
            if(++recvCmdErrorTime > 5)
            {
                recvCmdErrorTime = 0;
                //this->gprsLedCtrl(0);
                gprsLedTimer->stop();
                //rstCtrl(1);
                //sleep(5);
                //rstCtrl(0);
                InitialGprs();
            }
#endif
            dealGprsTalkError();
            if(strstr(gprsCmd, "OK"))
            {
                gprsInit = cmdMIPCLOSE;
                //gprsInit = cmdMIPOPEN;
                sleep(2);
                writeGprsCmd(gprsInit);
            }
            else if(strstr(gprsCmd, "ERROR"))
            {
                dealGprsTalkError();
                gprsInit = cmdCPMS;
                sleep(3);
                writeGprsCmd(gprsInit);
            }
            break;
        case cmdMIPCLOSE:         
            qDebug("kira --- gprs initial MIPCLOSE result");
            if(strstr(gprsCmd, "OK"))
            {
                //gprsInit = cmdMIPOPEN;
                gprsInit = cmdMIPCALL;
                sleep(2);
                writeGprsCmd(gprsInit);
            }
            else if(strstr(gprsCmd, "ERROR"))
            {
                sleep(3);
                gprsInit = cmdMIPDIS;
                writeGprsCmd(gprsInit);
            }
            break;
        case cmdMIPCALL:          
            qDebug("kira --- gprs initial MIPCALL result");
#if 1
            if(++recvCmdErrorTime > 5)
            {
                recvCmdErrorTime = 0;
                //this->gprsLedCtrl(0);
                gprsLedTimer->stop();
                InitialGprs();
            }
#endif
            if(strstr(gprsCmd, "+MIPCALL:0") || strstr(gprsCmd, "ERROR"))
            {
                gprsInit = cmdMIPDIS;
                //gprsInit = cmdMIPCLOSE;
                sleep(2);
                writeGprsCmd(gprsInit);
            }
            else if(strstr(gprsCmd, "+MIPCALL"))
            {
                gprsInit = cmdMIPOPEN;
                sleep(3);
                writeGprsCmd(gprsInit);
            }
            break;
        case cmdMIPOPEN:
            qDebug("kira --- gprs initial cmdMIPOPEN result");
            if(strstr(gprsCmd, "OK") || strstr(gprsCmd, "MIPOPEN") || strstr(gprsCmd, "MIPSTART"))
            {
                gprsInit = cmdMIPSETS;
                sleep(2);
                writeGprsCmd(gprsInit);
            }
            else if(strstr(gprsCmd, "ERROR"))
            {
                gprsInit = cmdMIPDIS;
                sleep(3);
                writeGprsCmd(gprsInit);
            }
            //writeGprsCmd(gprsInit);
            break;
        case cmdMIPSETS:
            qDebug("kira --- gprs initial cmdMIPSETS result");

#if 1
            if(strstr(gprsCmd, "OK"))
            {
                gprsInit = cmdMIPDSETS;
                sleep(2);
                writeGprsCmd(gprsInit);
            }
            else if(strstr(gprsCmd, "ERROR"))
            {
                gprsInit = cmdMIPDIS;
                sleep(3);
                writeGprsCmd(gprsInit);
            }
#endif
            //writeGprsCmd(gprsInit);
            break;
        case cmdMIPDSETS:
            qDebug("kira --- gprs initial cmdMIPDSETS result");

#if 1
            if(strstr(gprsCmd, "OK"))
            {
                gprsInit = finishINIT;
                qDebug("kira --- gprs initial success");
                sleep(3);
                emit sendBeatInfo();
                beatTimer->start();
                //this->gprsLedCtrl(1);
                gprsLedTimer->start();
                recvCmdErrorTime = 0;
            }
            else if(strstr(gprsCmd, "ERROR"))
            {
                gprsInit = cmdMIPDIS;
                sleep(3);
                writeGprsCmd(gprsInit);
            }
#endif
            break;
        case finishINIT:
#if 1
            //qDebug("kira --- gprs receive server data:");
            //QString recvData = QByteArray::fromRawData(gprsbuf, recvLen);
            //qDebug(qPrintable(recvData));

            memcpy(&gprsData[gprsDatalen], gprsbuf, recvLen);
            gprsDatalen += recvLen;
            if(gprsDatalen >= RECVDATA_LEN)
            {
                memset(gprsData, 0, sizeof(gprsData));
                gprsDatalen = 0;

                serverDataErrorCount(); // kira added - 2016.11.25

                break;
            }
            //qDebug("kira ---gprsDatalen: %d", gprsDatalen);
            //qDebug("kira --- gprsData[gprsDatalen-2]: %x", gprsData[gprsDatalen-2]);
            //qDebug("kira --- gprsData[gprsDatalen-1]: %x", gprsData[gprsDatalen-1]);

            if(gprsDatalen > 2)
            {
                if((gprsData[gprsDatalen-2] != 0x0d) && (gprsData[gprsDatalen-1] != 0x0a))
                    break;
            }
            else
            {
                serverDataErrorCount(); // kira added - 2016.11.25
                break;
            }

            qDebug("kira --- gprs receive full server data:");
            QString recvfullData = QByteArray::fromRawData(gprsData, gprsDatalen);
            //qDebug(qPrintable(recvfullData));

            bool left = false;
            //if(strstr(gprsData, "+MIPRTCP: 1,0,"))
            if(strstr(gprsData, "+MIPRTCP:"))
            {
again:
                int index = recvfullData.indexOf(',');
                recvfullData = recvfullData.mid(index+1);
                //qDebug(qPrintable(recvfullData));
                int index2 = recvfullData.indexOf(',');
                QString leftstr = recvfullData.mid(0, index2);
                if(leftstr.toInt() != 0)
                    left = true;
                //qDebug(qPrintable(leftstr));
                recvfullData = recvfullData.mid(index2+1);
                qDebug(qPrintable(recvfullData));

                //headlen = strlen("+MIPRTCP: 1,0,");
                memset(gprsData, 0, sizeof(gprsData));
                memcpy(gprsData, recvfullData.toLatin1().data(), recvfullData.length());
                qDebug("kira --- recv full data");
                //dataLenL = AscToHex(gprsData[headlen+8])*16 + AscToHex(gprsData[headlen+9]);
                //dataLenH = AscToHex(gprsData[headlen+10])*16 + AscToHex(gprsData[headlen+11]);
                dataLenL = AscToHex(gprsData[6])*16 + AscToHex(gprsData[7]);
                dataLenH = AscToHex(gprsData[8])*16 + AscToHex(gprsData[9]);
                recvDatalen = dataLenH*256 + dataLenL;
                if(recvDatalen > RECVDATA_LEN-7)
                {
                    if(left)
                        goto again;
                    serverDataErrorCount(); // kira added - 2016.11.25
                    break;
                }
                char recvdata[RECVDATA_LEN] = {0};
                for(int i=0; i<recvDatalen+7; i++)
                {
                    recvdata[i] = (char)(((AscToHex(gprsData[2*i])<<4) & 0xf0)
                            +(AscToHex(gprsData[2*i+1]) & 0x0f));
                    //recvdata[i] = (char)(((AscToHex(gprsData[2+headlen+2*i])<<4) & 0xf0)
                    //        +(AscToHex(gprsData[2+headlen+2*i+1]) & 0x0f));
                    //qDebug("kira --- recvdata[%d]: %x", i, recvdata[i]);
                }
                memset(gprsData, 0, sizeof(gprsData));
                gprsDatalen = 0;

                recvDataErrorTime = 0;
                initGprsTime = 0;
                if((recvdata[0] == 0x55) && (recvdata[1] == 0x7a))
                {
                    //int datalen;
                    //memcpy(&datalen, &gprsbuf[3], 2);
                    int check = 0;
                    for(int i=0; i<recvDatalen+6; i++)
                        check ^= recvdata[i];
                    if(check == recvdata[recvDatalen+6])
                    {
                        //fillServerData(&gprsbuf[6], datalen);
                        serverDataErrorTime = 0; // kira added - 2016.11.25
                        switch(recvdata[2])
                        {
                            case 0x01:  // 心跳回应
                                qDebug("kira ---process beat ack...");
                                fillBeatAckInfo(&recvdata[6], recvDatalen);
                                emit recvBeatAck_v1();
                                break;
                            case 0x02:  // 消费回应
                                qDebug("kira ---process consume ack...");
                                fillConsumeAckInfo(&recvdata[6], recvDatalen);
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
                                fillweixinBeatAckInfo(&recvdata[6], recvDatalen);
                                emit recvWeixinBeatAck_v1();
                                break;
                            case 0x22:  // 微信在线支付结果
                                qDebug("kira ---process consume ack...");
                                //fillweixinTradeAckInfo(&recvdata[6], recvDatalen);
                                fillOnlineTradeAckInfo(&recvdata[6], recvDatalen);
                                emit recvWeixinTradeAck_v1();
                                break;
                            case 0x23:  // Alipay key请求回应
                                qDebug("kira --- process Alipay keyAck...");
                                fillAlipayKeyInfo(&recvdata[6], recvDatalen);
                                emit recvAlipayKeyInfo_v1();
                                break;
                            case 0x24: // Alipay交易记录上传回应
                                qDebug("kira --- process Alipay tradeAck...");
                                fillAlipayTradeAckInfo(&recvdata[6], recvDatalen);
                                emit recvAlipayTradeAck_v1();
                                break;
                            case 0x25: // Alipay黑名单请求回应
                                qDebug("kira --- process Alipay blackAck...");
                                fillAlipayBlackInfo(&recvdata[6], recvDatalen);
                                emit recvAlipayBlackInfo_v1();
                                break;
                            case 0x29: // 司机签到信息回应
                                qDebug("kira --- process driver sign recordAck...");
                                fillDriverSignInfo(&recvdata[6], recvDatalen);
                                emit recvDriverSignInfo_v1();
                                break;
                        }
                    }
                    else
                    {
                        serverDataErrorCount(); // kira added - 2016.11.25
                        qDebug("kira --- gprs receive data check error!");
                    }
                }
                else
                {
                    serverDataErrorCount(); // kira added - 2016.11.25
                    qDebug("kira --- gprs receive data framehead error!");
                }

                if(left)
                {
                    qDebug("kira --- have left data have to analyse...");
                    goto again;
                }

            }
            else if(strstr(gprsData, "ERROR"))
            {
                if(++recvDataErrorTime > 3)
                {
                    recvDataErrorTime = 0;
                    InitialGprs();
                    //this->gprsLedCtrl(0);
                    gprsLedTimer->stop();
                }
                memset(gprsData, 0, sizeof(gprsData));
                gprsDatalen = 0;
            }
            else if(strstr(gprsData, "+MIPSEND:") || strstr(gprsData, "OK"))
            {
                qDebug("kira --- gprs send data success!");
                memset(gprsData, 0, sizeof(gprsData));
                gprsDatalen = 0;
            }
            else
            {
                memset(gprsData, 0, sizeof(gprsData));
                gprsDatalen = 0;
                break;
            }
#endif
            break;
    }
    recvLen = 0;
    memset(gprsbuf, 0, sizeof(gprsbuf));

    memset(gprsCmd, 0, sizeof(gprsCmd));
    gprsCmdlen = 0;
}

void gprsSerialTalk::writeData(char *data, int datalen)
{
    if(gprsfd > 0)
    {
        int wlen = write(gprsfd, data, datalen);
        qDebug("kira --- gprs write data datalen=%d, wlen=%d", datalen, wlen);
        //QString writeData = QByteArray::fromRawData(data, datalen).toHex();
        //qDebug(qPrintable(writeData));
        qDebug(data);
        myTimer->start();
    }
}

// kira added - 2016.11.25
void gprsSerialTalk::serverDataErrorCount()
{
    if(serverDataErrorTime++ > 5)
    {
        serverDataErrorTime;
        RestartGprs();
    }

}

// kira added - 2016.11.25
void gprsSerialTalk::RestartGprs()
{
    qDebug("kira --- restart gprs module...");
    powerctrlCtrl(0);
    sleep(3);
    powerctrlCtrl(1);
    sleep(2);
    powerCtrl(1);
    gprsPowerFlag = true;
}

void gprsSerialTalk::InitialGprs()
{
    if(++initGprsTime > 3)
    {
        RestartGprs();
    }
    sleep(3);
    myTimer->start();
    beatTimer->stop();
    gprsInit = cmdATE;
    writeGprsCmd(gprsInit);
}

void gprsSerialTalk::writeGprsCmd(gprsState state)
{
    switch(state)
    {
        case cmdATE:
            qDebug("kira --- gprs initial ATE cmd");
            writeData((char*)"ATE0\r\n", strlen("ATE0\r\n"));
            break;
        case cmdCPIN:
            qDebug("kira --- gprs initial CPIN cmd");
            writeData((char*)"AT+CPIN?\r\n", strlen("AT+CPIN?\r\n"));
            break;
        case cmdCGREG:
            qDebug("kira --- gprs initial CGREG cmd");
            writeData((char*)"AT+CGREG=1\r\n", strlen("AT+CGREG=1\r\n"));
            break;
        case cmdCNMI:
            qDebug("kira --- gprs initial CNMI cmd");
            writeData((char*)"AT+CNMI=1,1,0,0,0\r\n", strlen("AT+CNMI=1,1,0,0,0\r\n"));
            break;
        case cmdCMGF:
            qDebug("kira --- gprs initial CMGF cmd");
            writeData((char*)"AT+CMGF=1\r\n", strlen("AT+CMGF=1\r\n"));
            break;
        case cmdCSMP:
            qDebug("kira --- gprs initial CSMP cmd");
            writeData((char*)"AT+CSMP=17,167,0,0\r\n", strlen("AT+CSMP=17,167,0,0\r\n"));
            break;
        case queryCGREG:
            qDebug("kira --- gprs query CGREG cmd");
            writeData((char*)"AT+CGREG?\r\n", strlen("AT+CGREG?\r\n"));
            break;
        case cmdCPMS:
            qDebug("kira --- gprs initial CPMS cmd");
            writeData((char*)"AT+CPMS=\"SM\"\r\n", strlen("AT+CPMS=\"SM\"\r\n"));
            break;
        case cmdMIPCALL:
            qDebug("kira --- gprs initial MIPCALL cmd");
            writeData((char*)"AT+MIPCALL=1,\"cmnet\"\r\n", strlen("AT+MIPCALL=1,\"cmnet\"\r\n"));
            //sleep(3);
            //writeData((char*)"AT+CSQ\r\n", strlen("AT+CSQ\r\n"));
            break;
        case cmdMIPDIS:
            qDebug("kira --- gprs initial MIPDIS cmd");
            writeData((char*)"AT+MIPCALL=0,\"cmnet\"\r\n", strlen("AT+MIPCALL=0,\"cmnet\"\r\n"));
            break;
        case cmdMIPCLOSE:
            qDebug("kira --- gprs initial MIPCLOSE cmd");
            writeData((char*)"AT+MIPCLOSE=1\r\n", strlen("AT+MIPCLOSE=1\r\n"));
            break;
        case cmdMIPSETS:
            qDebug("kira --- gprs initial MIPSETS cmd");
            writeData((char*)"AT+MIPSETS=1,1372,1\r\n", strlen("AT+MIPSETS=1,1372,1\r\n"));
            break;
        case cmdMIPDSETS:
            qDebug("kira --- gprs initial MIPDSETS cmd");
            writeData((char*)"AT+MIPDSETS=1,1372,300\r\n", strlen("AT+MIPDSETS=1,1372,300\r\n"));
            break;
        case cmdMIPOPEN:
            qDebug("kira --- gprs initial MIPOPEN cmd");
            char temp[100] = {0};
            QByteArray iptemp = serverIP.toLatin1();
            sprintf(temp, "AT+MIPOPEN=1,1200,\"%s\",%d,0\r\n", iptemp.data(), serverPort.toInt());
            writeData(temp, strlen(temp));
            //writeData((char*)"ATD13646815584;\r\n", strlen("ATD13646815584\r\n"));
            break;
    }
}

int gprsSerialTalk::sendGprsData(char *data, int datalen)
{        
    if(gprsInit == finishINIT)
    {
        QByteArray datatemp = QByteArray(data, datalen).toHex();
        char temp[4096] = {0};
        sprintf(temp, "AT+MIPSEND=1,\"%s\"\r\n", datatemp.data());
        writeData(temp, strlen(temp));
        return 0;
    }
    else
    {
       qDebug("kira --- pos disconnect from server!");
       return 1;
    }
}

void gprsSerialTalk::fillBeatAckInfo(char *pdata, int dataLen)
{
    QMutexLocker locker(&beatMutex);

    memset(&beatAckInfo, 0, sizeof(beatAckInfo_t));
    memcpy(&beatAckInfo, pdata, dataLen);
}

void gprsSerialTalk::readBeatAckInfo(beatAckInfo_t *pbeatAckInfo)
{
    QMutexLocker locker(&beatMutex);

    memcpy(pbeatAckInfo, &beatAckInfo, sizeof(beatAckInfo_t));
}

void gprsSerialTalk::fillConsumeAckInfo(char *pdata, int dataLen)
{
    QMutexLocker locker(&consumeMutex);

    memset(&consumeAckInfo, 0, sizeof(consumeAckInfo_t));
    memcpy(&consumeAckInfo, pdata, dataLen);
}

void gprsSerialTalk::readConsumeAckInfo(consumeAckInfo_t *pconsumeAckInfo)
{
    QMutexLocker locker(&consumeMutex);

    memcpy(pconsumeAckInfo, &consumeAckInfo, sizeof(consumeAckInfo_t));
}

// 微信在线支付
void gprsSerialTalk::fillweixinTradeAckInfo(char *pdata, int dataLen)
{
    QMutexLocker locker(&consumeMutex);
    qDebug("kira --- pdata： %x", pdata[0]);
    memset(&weixinTradeAck, 0, sizeof(weixinTradeAck_t));
    memcpy(&weixinTradeAck, pdata, dataLen);
}

void gprsSerialTalk::readweixinTradeAckInfo(weixinTradeAck_t* pweixinTradeAck)
{
    QMutexLocker locker(&consumeMutex);

    memcpy(pweixinTradeAck, &weixinTradeAck, sizeof(weixinTradeAck_t));
}

void gprsSerialTalk::fillOnlineTradeAckInfo(char *pdata, int dataLen)
{
    QMutexLocker locker(&consumeMutex);
    qDebug("kira --- online pdata： %x", pdata[4]);
    memset(&onlineTradeAck, 0, sizeof(onlineTradeAck_t));
    memcpy(&onlineTradeAck, pdata, dataLen);
}

void gprsSerialTalk::readOnlineTradeAckInfo(onlineTradeAck_t* ponlineTradeAck)
{
    QMutexLocker locker(&consumeMutex);

    memcpy(ponlineTradeAck, &onlineTradeAck, sizeof(onlineTradeAck_t));
}

void gprsSerialTalk::fillweixinBeatAckInfo(char *pdata, int dataLen)
{
    QMutexLocker locker(&beatMutex);

    memset(&weixinBeatAck, 0, sizeof(weixinBeatAck_t));
    memcpy(&weixinBeatAck, pdata, dataLen);
}

void gprsSerialTalk::readweixinBeatAckInfo(weixinBeatAck_t* pweixinBeatAck)
{
    QMutexLocker locker(&beatMutex);

    memcpy(pweixinBeatAck, &weixinBeatAck, sizeof(weixinBeatAck_t));
}


void gprsSerialTalk::gprsLedCtrl(int flag)
{
    if(gprsLedfd > 0)
    {
        if(flag)
            write(gprsLedfd, "1", 1);
        else
            write(gprsLedfd, "0", 1);
    }
}

void gprsSerialTalk::powerCtrl(int flag)
{
    if(powerfd > 0)
    {
        if(flag)
            write(powerfd, "1", 1);
        else
            write(powerfd, "0", 1);
    }
}

void gprsSerialTalk::powerctrlCtrl(int flag)
{
    if(powerctrlfd > 0)
    {
        if(flag)
            write(powerctrlfd, "1", 1);
        else
            write(powerctrlfd, "0", 1);
    }
}

void gprsSerialTalk::rstCtrl(int flag)
{
    if(rstfd > 0)
    {
        if(flag)
            write(rstfd, "1", 1);
        else
            write(rstfd, "0", 1);
    }
}

char gprsSerialTalk::AscToHex(char aChar)
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

#if 0
void gprsSerialTalk::fillServerData(char *pdata, int dataLen)
{
    // 需要添加互斥锁
    memset(serverData, 0, SERVERDATA_LEN);
    serverDataLen = dataLen;
    memcpy(serverData, pdata, dataLen);
}

void gprsSerialTalk::readServerData(char *pdata, int *pdataLen)
{
    // 需要添加互斥锁
    memcpy(pdata, serverData, SERVERDATA_LEN);
    *pdataLen = serverDataLen;
}
#endif

// Alipay
void gprsSerialTalk::fillAlipayKeyInfo(char *pdata, int dataLen)
{
    QMutexLocker locker(&alipaykeyMutex);

    memset(&alipayKeyInfo, 0, sizeof(alipayKeyInfo_t));
    memcpy(&alipayKeyInfo, pdata, dataLen);
}

void gprsSerialTalk::readAlipayKeyInfo(alipayKeyInfo_t* palipayKeyInfo)
{
    QMutexLocker locker(&alipaykeyMutex);

    memcpy(palipayKeyInfo, &alipayKeyInfo, sizeof(alipayKeyInfo_t));
}

void gprsSerialTalk::fillAlipayBlackInfo(char *pdata, int dataLen)
{
    QMutexLocker locker(&alipayblackMutex);

    memset(&alipayBlackInfo, 0, sizeof(alipayBlackInfo_t));
    memcpy(&alipayBlackInfo, pdata, dataLen);
}

void gprsSerialTalk::readAlipayBlackInfo(alipayBlackInfo_t* palipayBlackInfo)
{
    QMutexLocker locker(&alipayblackMutex);

    memcpy(palipayBlackInfo, &alipayBlackInfo, sizeof(alipayBlackInfo_t));
}

void gprsSerialTalk::fillAlipayTradeAckInfo(char *pdata, int dataLen)
{
    QMutexLocker locker(&alipaytradeMutex);

    memset(&alipayTradeInfoAck, 0, sizeof(alipayTradeInfoAck_t));
    memcpy(&alipayTradeInfoAck, pdata, dataLen);
}

void gprsSerialTalk::readAlipayTradeAckInfo(alipayTradeInfoAck_t *palipayTradeInfoAck)
{
    QMutexLocker locker(&alipaytradeMutex);

    memcpy(palipayTradeInfoAck, &alipayTradeInfoAck, sizeof(alipayTradeInfoAck_t));
}

void gprsSerialTalk::fillDriverSignInfo(char *pdata, int dataLen)
{
    QMutexLocker locker(&driverSignMutex);

    memset(&driveSignNetAck, 0, sizeof(driveSignNetAck_t));
    memcpy(&driveSignNetAck, pdata, dataLen);
}

void gprsSerialTalk::readDriverSignInfo(driveSignNetAck_t *pdriveSignNetAck)
{
    QMutexLocker locker(&driverSignMutex);

    memcpy(pdriveSignNetAck, &driveSignNetAck, sizeof(driveSignNetAck_t));
}
