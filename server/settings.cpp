#include <QApplication>
#include "settings.h"

Settings *Settings::self = 0;

Settings::Settings(const QString &fileName, Format format, QObject *parent) :
    QSettings(fileName, format, parent)
{
}

Settings *Settings::instance()
{
    if (!self)
        self = new Settings(qApp->applicationDirPath() + "/config.ini");
    return self;
}

void Settings::setBrightness(int value)
{
    setValue("Brightness", value);
}

int Settings::getBrightness()
{
    return value("Brightness", 4).toInt();
}

void Settings::setVolume(int value)
{
    setValue("Volume", value);
    emit volumeChanged(value);
}

int Settings::getVolume()
{
    return value("Volume", 50).toInt();
}

int Settings::getIdleTime()
{
    return value("IdleTime", 5).toInt();
}

QTime Settings::getIdleTimeFrom() const
{
    return value("IdleTimeFrom", QTime(23, 0, 0)).toTime();
}
QTime Settings::getIdleTimeTo() const
{
    return value("IdleTimeTo", QTime(6, 0, 0)).toTime();
}

void Settings::setIdleTime(int value)
{
    setValue("IdleTime", value);
    emit idleTimeChanged(value);
}

void Settings::setIdleTimeFrom(const QTime &value)
{
    setValue("IdleTimeFrom", value);
    emit idleTimeFromChanged(value);
}

void Settings::setIdleTimeTo(const QTime &value)
{
    setValue("IdleTimeTo", value);
    emit idleTimeToChanged(value);
}
