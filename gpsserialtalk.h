#ifndef GPSSERIALTALK_H
#define GPSSERIALTALK_H

#include <QSocketNotifier>
#include <QMessageBox>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "serialdata.h"
#include <QMutex>
#include <QMutexLocker>
#ifndef Q_MOC_RUN
#include "../boost_1_58_0/boost_include.h"
#endif

class QTimer;

struct gpsSerialInfo
{
    unsigned char datetime[30];
    unsigned char longitude[30];
    unsigned char latitude[30];
    unsigned char speed[30];
    unsigned char direction[30];
    unsigned char starnum[30];
    unsigned char longitude_sign[30];
    unsigned char latitude_sign[30];
};

class gpsSerialTalk : public QWidget
{
    Q_OBJECT
public:
    explicit gpsSerialTalk(QWidget *parent = 0);

signals:

public slots:

public:
    void getGpsInfo(gpsSerialInfo* pgpsInfo);
    //Write some data to port
    void write_to_serial(const char* data, int isize);
    //Read data from port which write data just now
    void read_from_serial();
    //To Call io_service::run function to call asyanc callback funciton
    void call_handle(int timeout_millisec);

protected:
    bool GNRMCAnalysis(std::string _RecString,gpsSerialInfo& gps);
    double GPSTransforming(std::string _Value);
    void handle_read(char* buf,boost::system::error_code ec,std::size_t bytes_transferred);
    //中国坐标内
    bool outofChina(double lat, double lon);
    QMutex dataMutex;
    boost::thread m_thr;
    void Run(void* p);
    bool m_bHaveData;

protected:
    //io_service Object
    boost::asio::io_service m_ios;
    //Serial port Object
    boost::asio::serial_port *pSerialPort;
    //Serial_port function exception
    boost::system::error_code ec;
    char m_buf[1024];
    boost::asio::deadline_timer* pTimer;
    bool m_bSucess;
    gpsSerialInfo gpsInfo;
};

#endif // GPSSERIALTALK_H
