#include <QFile>

#include "settings.h"
#include "hal.h"


Hal *Hal::self = 0;

const char *Hal::RedLed = "/sys/class/gpio/gpio295/value";
const char *Hal::YellowLed = "/sys/class/gpio/gpio296/value";
const char *Hal::BlueLed = "/sys/class/gpio/gpio296/value";

const char *Hal::brightness = "/sys/class/backlight/pwm-backlight.0/brightness";
const char *Hal::maxBrightnes = "/sys/class/backlight/pwm-backlight.0/max_brightness";

Hal::Hal(QObject *parent) :
    QObject(parent),
    isPowerOff(false)
{

}

Hal *Hal::instance()
{
    if (!self)
        self = new Hal();
    return self;
}

void Hal::powerOn()
{
    if (isPowerOff) {
        isPowerOff = false;
        setBrightness(Settings::instance()->getBrightness());
    }
}

void Hal::powerOff()
{
    if (!isPowerOff) {
        isPowerOff = true;
        setBrightness(0);
    }
}

void Hal::togglePower()
{
    if (isPowerOff)
        powerOn();
    else
        powerOff();
}

void Hal::setValue(const QString &sysfs, int value)
{
    QFile file(sysfs);
    if (file.open(QIODevice::WriteOnly | QIODevice::Unbuffered)) {
        file.write(QByteArray::number(value));
        file.close();
    }
}

int Hal::getValue(const QString &sysfs)
{
    QFile file(sysfs);
    if (file.open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
        QByteArray buf = file.readAll().trimmed();
        file.close();
        return buf.toInt();
    }
    return -1;
}
