#ifndef SETCONFIG_H
#define SETCONFIG_H

#include <QSettings>

class setConfig
{
public:
    setConfig();
    void writeSetInfo(const QString setfile, const QString group, const QString wstr, const QString wdata);
    void readSetInfo(const QString setfile, const QString group, const QString rstr, QString& rdata);
};

#endif // SETCONFIG_H
