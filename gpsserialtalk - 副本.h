#ifndef GPSSERIALTALK_H
#define GPSSERIALTALK_H

#include <QSocketNotifier>
#include <QMessageBox>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "serialdata.h"
#include <QThread>
#include <QMutex>
#include <QMutexLocker>

class QTimer;
#define MAX_GPSBUF_LEN  2048

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

class gpsSerialTalk : public QThread
{
    Q_OBJECT
public:
    explicit gpsSerialTalk();

signals:

public slots:
    void readData();
    void onTimerOut();

public:
    bool openSerial();
    void getGpsInfo(gpsSerialInfo* pgpsInfo);
    void gpsPowerCtrl(int flag);
    SerialData gpsSerialData;
    QSocketNotifier* gpsMonitor;
    int gpsfd;
    int gpsPowerfd;
    gpsSerialInfo gpsInfo;
    char gpsbuf[MAX_GPSBUF_LEN];
    int recvLen;
    bool GNRMCAnalysis(std::string _RecString, gpsSerialInfo& gps);
    double GPSTransforming(std::string _Value);
    //中国坐标内
    bool outofChina(double lat, double lon);

    QTimer* myTimer;
    QMutex dataMutex;

protected:
    void run();

public:
    void stop();

private:
    bool m_stopFlag;
    QMutex threadMutex;
};

#endif // GPSSERIALTALK_H
