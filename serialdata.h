#ifndef SERIALDATA_H
#define SERIALDATA_H

#include <termios.h>    // linux
#include <QString>

struct SerialData
{
    QString devName;
    QString baudRate;
    QString dataBit;
    QString parity;
    QString stopBit;
};
#endif // SERIALDATA_H

