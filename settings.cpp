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
