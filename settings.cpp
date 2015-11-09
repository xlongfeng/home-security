#include "settings.h"

Settings *Settings::self = 0;

Settings::Settings(const QString &fileName, Format format, QObject *parent) :
    QSettings(fileName, format, parent)
{
}

Settings *Settings::instance()
{
    if (!self)
        self = new Settings("config.ini");
    return self;
}
