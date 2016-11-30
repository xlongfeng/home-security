#ifndef HAL_H
#define HAL_H

#include <QObject>

class Hal : public QObject
{
    Q_OBJECT

public:
    static Hal *instance();

    bool isPowerOn()
    {
        return !isPowerOff;
    }

    void powerOn();
    void powerOff();
    void togglePower();

    void setRedLed(int value)
    {
        setValue(RedLed, value);
    }

    void setYellowLed(int value)
    {
        setValue(YellowLed, value);
    }

    void setBlueLed(int value)
    {
        setValue(BlueLed, value);
    }

    void setBrightness(int value)
    {
        setValue(brightness, value);
    }

    int getMaxBrightness()
    {
        int value = getValue(maxBrightnes);
        if (value == -1)
            value = 2;
        return value;
    }

private:
    explicit Hal(QObject *parent = 0);
    Q_DISABLE_COPY(Hal)
    void setValue(const QString &sysfs, int value);
    int getValue(const QString &sysfs);

private:
    static Hal *self;

    bool isPowerOff;

    static const char *RedLed;
    static const char *YellowLed;
    static const char *BlueLed;

    static const char *brightness;
    static const char *maxBrightnes;
};

#endif // HAL_H
