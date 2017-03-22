#include "gpsserialtalk.h"
#include "../boost_1_58_0/boost_include.h"
#include "string"
#include "vector"
#include <QTimer>

gpsSerialTalk::gpsSerialTalk()
{
    qDebug("kira --- Initial gps");
    gpsSerialData.devName = "ttySP4";
    gpsSerialData.baudRate = "B9600";
    gpsSerialData.dataBit = "8";
    gpsSerialData.parity = "none";
    gpsSerialData.stopBit = "1";
    recvLen = 0;
    memset(gpsbuf, 0, sizeof(gpsbuf));
    this->openSerial();

    m_stopFlag = false;
    recvLen = 0;

#if 1
    //新建一个QTimer对象
    myTimer = new QTimer();
    //设置定时器每个多少毫秒发送一个timeout()信号
    myTimer->setInterval(1000);
    //启动定时器
    myTimer->stop();
    connect(myTimer, SIGNAL(timeout()), this, SLOT(onTimerOut()), Qt::DirectConnection);
#endif

    gpsPowerfd = open("/sys/class/gpio/gpio77/value", O_RDWR);
    if(gpsPowerfd < 0)
    {
        qDebug("kira --- open gps power failed: %d", gpsPowerfd);
    }
    gpsPowerCtrl(1);
}

void gpsSerialTalk::gpsPowerCtrl(int flag)
{
    if(gpsPowerfd > 0)
    {
        if(flag)
            write(gpsPowerfd, "1", 1);
        else
            write(gpsPowerfd, "0", 1);
    }
}

bool gpsSerialTalk::openSerial()
{
    struct termios setting;
    gpsfd = open(qPrintable("/dev/" + gpsSerialData.devName), O_RDWR);
    if (gpsfd == -1) {
        //QMessageBox::warning(this, "gps open error", strerror(errno));
        goto openerr;
    }
    tcgetattr(gpsfd, &setting);
    //  设置波特率
    speed_t speed;
    if (gpsSerialData.baudRate == "B4800") {
        speed = B4800;
    } else if (gpsSerialData.baudRate == "B9600") {
        speed = B9600;
    } else if (gpsSerialData.baudRate == "B19200") {
        speed = B19200;
    } else if (gpsSerialData.baudRate == "B38400") {
        speed = B38400;
    } else if (gpsSerialData.baudRate == "B57600") {
        speed = B57600;
    } else if (gpsSerialData.baudRate == "B115200") {
        speed = B115200;
    } else if (gpsSerialData.baudRate == "B230400") {
        speed = B230400;
    } else if (gpsSerialData.baudRate == "B460800") {
        speed = B460800;
    } else {
        //QMessageBox::warning(this, "error", "gps speed error!");
        goto seterr;
    }
    cfsetispeed(&setting, speed);
    cfsetospeed(&setting, speed);
    cfmakeraw(&setting);
    //  设置数据位
    if (gpsSerialData.dataBit == "8") {
        setting .c_cflag |= CS8;
    } else if (gpsSerialData.dataBit == "7") {
        setting.c_cflag |= CS7;
    } else if (gpsSerialData.dataBit == "6") {
        setting.c_cflag |= CS6;
    } else if (gpsSerialData.dataBit == "5") {
        setting.c_cflag |= CS5;
    } else {
        //QMessageBox::warning(this, "error", "gps dataBit error!");
        goto seterr;
    }
    //  设置parity
    if (gpsSerialData.parity == "none") {
        setting.c_cflag &= ~PARENB;
        setting.c_iflag &= ~INPCK;
    } else if (gpsSerialData.parity == "odd") {
        setting.c_cflag |= (PARODD | PARENB);
        setting.c_iflag |= INPCK;
    } else if (gpsSerialData.parity == "even") {
        setting.c_cflag |= PARENB;
        setting.c_cflag &= ~PARODD;
        setting.c_iflag |= INPCK;
    } else {
        //QMessageBox::warning(this, "error", "gps dataBit error!");
        goto seterr;
    }
    //  设置停止位
    if (gpsSerialData.stopBit == "1") {
        setting.c_cflag &= ~CSTOPB;
    } else if (gpsSerialData.stopBit == "2") {
        setting.c_cflag |= CSTOPB;
    } else {
        //QMessageBox::warning(this, "error", "gps stopBit error!");
        goto seterr;
    }

    tcflush(gpsfd, TCIFLUSH);
    setting.c_cc[VTIME] = 0;
    setting.c_cc[VMIN] = 0;
    tcsetattr(gpsfd, TCSANOW, &setting);

#if 0
    gpsMonitor = new QSocketNotifier(gpsfd, QSocketNotifier::Read, NULL);
    connect(gpsMonitor, SIGNAL(activated(int)), this, SLOT(readData()));
    gpsMonitor->setEnabled(true);
#endif

    return true;
seterr:
    ::close(gpsfd);
openerr:
    return false;
}

void gpsSerialTalk::readData()
{  
    recvLen += read(gpsfd, gpsbuf+recvLen, MAX_GPSBUF_LEN-recvLen);
    if(recvLen >= MAX_GPSBUF_LEN)
    {
        recvLen = 0;
    }

    //QMutexLocker locker(&dataMutex);

#if 0
    std::string line = gpsbuf;
    gpsSerialInfo tag;
    printf("kira --- gps line: %s \n", line.c_str());
    memset(&tag,0,sizeof(gpsSerialInfo));
    if(GNRMCAnalysis(line, tag))
    {
        QMutexLocker locker(&dataMutex);
        //copy tmpvalue
        memset(&gpsInfo,0,sizeof(gpsSerialInfo));
        memcpy(&gpsInfo,&tag,sizeof(gpsSerialInfo));
    }
 #endif

#if 1
    std::string lines = gpsbuf;
    std::vector<std::string> vec;
    boost::split(vec, lines, boost::is_any_of("$"), boost::token_compress_on); // v2 --- 2017.3.14
    for(int i=0; i<vec.size(); ++i)
    {
        gpsSerialInfo tag;
        memset(&tag, 0, sizeof(gpsSerialInfo));
        if(GNRMCAnalysis(vec[i], tag))
        {
            QMutexLocker locker(&dataMutex);
            //copy tmpvalue
            memset(&gpsInfo, 0, sizeof(gpsSerialInfo));
            memcpy(&gpsInfo, &tag, sizeof(gpsSerialInfo));
        }
    }
#endif
    //memset(gpsbuf, 0, sizeof(gpsbuf));
    //recvLen = 0;
}

void gpsSerialTalk::onTimerOut()
{
    myTimer->stop();

    QMutexLocker locker(&dataMutex);
#if 0
    std::string line = gpsbuf;
    gpsSerialInfo tag;
    memset(&tag,0,sizeof(gpsSerialInfo));
    if(GNRMCAnalysis(line,tag))
    {
        QMutexLocker locker(&dataMutex);
        //copy tmpvalue
        memset(&gpsInfo,0,sizeof(gpsSerialInfo));
        memcpy(&gpsInfo,&tag,sizeof(gpsSerialInfo));
    }
#endif

    memset(gpsbuf, 0, sizeof(gpsbuf));
    recvLen = 0;

    myTimer->start();
}

void gpsSerialTalk::stop()
{
    QMutexLocker locker(&threadMutex);
    m_stopFlag = true;
}

void gpsSerialTalk::run()
{
    int ret = 0;
    fd_set  sets;
    struct timeval val;

    //myTimer->start();

    while(1)
    {
        QMutexLocker locker(&threadMutex);
        FD_ZERO(&sets);
        FD_SET(gpsfd, &sets);

        val.tv_sec = 0;
        val.tv_usec = 5000;
        ret = select(gpsfd+1, &sets, NULL, NULL, &val);
        if (ret == -1)
        {
            ;
        }
        else if(ret)
        {
            usleep(20);
            if(FD_ISSET(gpsfd, &sets))
            {
                readData();
            }
        }

        if(m_stopFlag)
            break;

        sleep(5);
    }
    m_stopFlag = false;
}

void gpsSerialTalk::getGpsInfo(gpsSerialInfo *pgpsInfo)
{
    QMutexLocker locker(&dataMutex);
    memcpy(pgpsInfo, &gpsInfo, sizeof(gpsSerialInfo));
}

double gpsSerialTalk::GPSTransforming(std::string _Value)
{
    double Ret = 0.0;
    std::vector<std::string> TempStr;
    //printf("kira --- GPS Analysis: %s \n", _Value.c_str());
    boost::split(TempStr, _Value,boost::is_any_of("."), boost::token_compress_on);

    if(TempStr.size() < 2)
    {
        qDebug("kira --- TempStr.size() < 2");
        return (0.0);
    }

    if(TempStr[0].length()<4 || TempStr[1].length()<5)
    {
        qDebug("kira --- TempStr[0].length()<4, TempStr[1].length()<5");
        return (0.0);
    }

    std::string x = TempStr[0].substr(0, TempStr[0].length()-2);
    std::string y = TempStr[0].substr(TempStr[0].length()-2, 2);
    std::string z = TempStr[1].substr(0, 5);
    if(z.length() != 5)
    {
        printf("%s %d\n", z.c_str(), z.length());
    }
    Ret = atof(x.c_str()) + atof(y.c_str()) / 60 + atof(z.c_str()) / 6000000;
    return Ret;

}

bool gpsSerialTalk::GNRMCAnalysis(std::string _RecString, gpsSerialInfo& gps)
{
    std::vector<std::string> vec;
    boost::split(vec, _RecString,boost::is_any_of(","), boost::token_compress_off);
    if(vec.size() < 10)
        return false;
    vec[0].erase(remove_if(vec[0].begin(), vec[0].end(), isspace), vec[0].end());
    if(vec[0].compare("GNRMC") != 0)
        return false;

    //printf("kira --- gps analysis: %s \n", _RecString.c_str());
    //时间
    vec[1].erase(remove_if(vec[1].begin(), vec[1].end(), isspace), vec[1].end());
    //纬度
    vec[3].erase(remove_if(vec[3].begin(), vec[3].end(), isspace), vec[3].end());
    if(vec[3].empty())
        return false;
    double dlat = GPSTransforming(vec[3]);
    vec[4].erase(remove_if(vec[4].begin(), vec[4].end(), isspace), vec[4].end());
    sprintf((char*)gps.latitude, "%.5f%s", dlat, vec[4].c_str());
    //经度
    vec[5].erase(remove_if(vec[5].begin(), vec[5].end(), isspace), vec[5].end());
    if(vec[5].empty())
        return false;
    double dlon = GPSTransforming(vec[5]);
    vec[6].erase(remove_if(vec[6].begin(), vec[6].end(), isspace), vec[6].end());
    sprintf((char*)gps.longitude,"%.5f%s",dlon,vec[6].c_str());
    if(outofChina(dlat, dlon)){
        printf("qwy --- out of china\n");
        return false;
    }
    //速度
    vec[7].erase(remove_if(vec[7].begin(), vec[7].end(), isspace), vec[7].end());
    if(boost::all(vec[7],boost::is_digit()|| boost::is_any_of(".")))
    {
        double dspeed = atof(vec[7].c_str()) * 1.852;
        sprintf((char*)gps.speed,"%.2f",dspeed);
    }
    //方向
    vec[8].erase(remove_if(vec[8].begin(), vec[8].end(), isspace), vec[8].end());
    if(boost::all(vec[8],boost::is_digit()|| boost::is_any_of(".")))
    {
        double dangle = atof(vec[8].c_str());
        sprintf((char*)gps.direction,"%.1f",dangle);
    }
    //日期
    vec[9].erase(remove_if(vec[9].begin(), vec[9].end(), isspace), vec[9].end());
    //--------------------------------------------------------------------------
    if(vec[1].length()>=6 && vec[9].length()>=6)
    {
        if(boost::all(vec[1],boost::is_digit()|| boost::is_any_of(".")) && boost::all(vec[9],boost::is_digit()|| boost::is_any_of(".")))
        {
            std::string datetime = "20";
            datetime += vec[9].substr(4,2);
            datetime += vec[9].substr(2,2);
            datetime += vec[9].substr(0,2);
            datetime += "T";
            datetime += vec[1].substr(0,2);
            datetime += vec[1].substr(2,2);
            datetime += vec[1].substr(4,2);
            boost::posix_time::ptime p = boost::posix_time::from_iso_string(datetime);
            p += boost::posix_time::hours(8);
            std::string dt = boost::posix_time::to_simple_string(p);
            memcpy(gps.datetime,dt.c_str(),dt.length());
        }
    }
    //-------------------------------------------------------------------------
    return true;
}

bool gpsSerialTalk::outofChina(double lat, double lon)
{
    if (lon < 72.004 || lon > 137.8347)
        return true;
    if (lat < 0.8293 || lat > 55.8271)
        return true;
    return false;
}

