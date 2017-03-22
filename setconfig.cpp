#include "setconfig.h"

setConfig::setConfig()
{

}

void setConfig::readSetInfo(const QString setfile, const QString group, const QString rstr, QString& rdata)
{
    QSettings settings(setfile, QSettings::IniFormat);
    settings.beginGroup(group);
    rdata = settings.value(rstr).toString();
    settings.endGroup();
}

void setConfig::writeSetInfo(const QString setfile, const QString group, const QString wstr, const QString wdata)
{
    QSettings settings(setfile, QSettings::IniFormat);
    settings.beginGroup(group);
    settings.setValue(wstr, wdata);
    settings.endGroup();
}


