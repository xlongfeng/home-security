#include <QTimer>
#include <QDebug>

#include "multipointcom.h"
#include "settings.h"
#include "watertower.h"


const quint8 WaterTowerIdentityBase = 0x10;

const int WaterTower::MaxQuantity = 6;
quint8 WaterTower::sampleInterval = 10;

QMap<int, WaterTower*> WaterTower::instanceMap;

WaterTower::WaterTower(quint8 id, QObject *parent) :
    QObject(parent),
    identity(id),
    com(new MultiPointCom()),
    height(0),
    waterLevel(0),
    isConnected(false),
    isAlarm(false)
{
    com->setAddress(WaterTowerIdentityBase + getAddress());
    connect(com, SIGNAL(responseReceived(char,QByteArray)), this, SLOT(responseReceived(char,QByteArray)));
    connect(com, SIGNAL(deviceConnected()), this, SIGNAL(deviceConnected()));
    connect(com, SIGNAL(deviceDisconnected()), this, SIGNAL(deviceDisconnected()));
    connect(com, SIGNAL(deviceConnected()), this, SLOT(deviceConnect()));
    connect(com, SIGNAL(deviceDisconnected()), this, SLOT(deviceDisconnect()));

    getSampleInterval();
    getLevelSensorHeight();
    getSensorNumber();

    height = levelSensorHeight * numberOfSensors;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(trigger()));
    timer->setSingleShot(true);

    enabled = isEnabled();
    if (enabled) {
        timer->start(3 * 1000);
    }

    alarmEnabled = isAlarmEnabled();
}

quint8 WaterTower::getAddress()
{
    quint8 address;
    Settings::instance()->beginGroup(QString("WaterTower-%1").arg(identity));
    address = Settings::instance()->value("address", 0xFF).toUInt();
    Settings::instance()->endGroup();
    return address;
}

void WaterTower::setAddress(quint8 address)
{
    Settings::instance()->beginGroup(QString("WaterTower-%1").arg(identity));
    Settings::instance()->setValue("address", address);
    Settings::instance()->endGroup();
    com->setAddress(WaterTowerIdentityBase + address);
}

bool WaterTower::isEnabled() const
{
    bool enable;
    Settings::instance()->beginGroup(QString("WaterTower-%1").arg(identity));
    enable = Settings::instance()->value("enable", false).toBool();
    Settings::instance()->endGroup();
    return enable;
}

void WaterTower::setEnable(bool enable)
{
    if (enabled != enable) {
        enabled = enable;
        timer->start(3 * 1000);
        Settings::instance()->beginGroup(QString("WaterTower-%1").arg(identity));
        Settings::instance()->setValue("enable", enable);
        Settings::instance()->endGroup();
    } else {
        timer->stop();
    }
}

bool WaterTower::isAlarmEnabled() const
{
    bool enable;
    Settings::instance()->beginGroup(QString("WaterTower-%1").arg(identity));
    enable = Settings::instance()->value("alarm", false).toBool();
    Settings::instance()->endGroup();
    return enable;
}

void WaterTower::setAlarmEnable(bool enable)
{
    if (alarmEnabled != enable) {
        alarmEnabled = enable;
        Settings::instance()->beginGroup(QString("WaterTower-%1").arg(identity));
        Settings::instance()->setValue("alarm", enable);
        Settings::instance()->endGroup();
    }
}

void WaterTower::setRadius(int centimetre)
{
    Settings::instance()->beginGroup(QString("WaterTower-%1").arg(identity));
    Settings::instance()->setValue("radius", centimetre);
    Settings::instance()->endGroup();
}

int WaterTower::getRadius()
{
    Settings::instance()->beginGroup(QString("WaterTower-%1").arg(identity));
    int radius = Settings::instance()->value("radius", 100).toInt();
    Settings::instance()->endGroup();
    return radius;
}

void WaterTower::setLevelSensorHeight(int centimetre)
{
    levelSensorHeight = centimetre;
    height = levelSensorHeight * numberOfSensors;
    emit waterLevelRangeChanged(0, height);
    Settings::instance()->beginGroup(QString("WaterTower-%1").arg(identity));
    Settings::instance()->setValue("level-sensor-height", levelSensorHeight);
    Settings::instance()->endGroup();
}

int WaterTower::getLevelSensorHeight()
{
    Settings::instance()->beginGroup(QString("WaterTower-%1").arg(identity));
    levelSensorHeight = Settings::instance()->value("level-sensor-height", 40).toInt();
    Settings::instance()->endGroup();
    return levelSensorHeight;
}

void WaterTower::setSensorNumber(int number)
{
    numberOfSensors = number;
    height = levelSensorHeight * numberOfSensors;
    emit waterLevelRangeChanged(0, height);
    Settings::instance()->beginGroup(QString("WaterTower-%1").arg(identity));
    Settings::instance()->setValue("number-of-sensors", numberOfSensors);
    Settings::instance()->endGroup();
}

int WaterTower::getSensorNumber()
{
    Settings::instance()->beginGroup(QString("WaterTower-%1").arg(identity));
    numberOfSensors = Settings::instance()->value("number-of-sensors", 8).toInt();
    Settings::instance()->endGroup();
    return numberOfSensors;
}

/* static */
void WaterTower::setSampleInterval(quint8 second)
{
    if (sampleInterval != second) {
        sampleInterval = second;
        Settings::instance()->setValue("WaterTowerSampleInterval", sampleInterval);
    }
}

/* static */
quint8 WaterTower::getSampleInterval()
{
    sampleInterval = Settings::instance()->value("WaterTowerSampleInterval", 10).toUInt();
    return sampleInterval;
}

/* static */
WaterTower *WaterTower::instance(int identity)
{
    WaterTower *wt;

    if (instanceMap.contains(identity)) {
        wt = instanceMap.value(identity);
    } else {
        wt = new WaterTower(identity);
        instanceMap[identity] = wt;
    }
    return wt;
}

void WaterTower::responseReceived(char protocol, const QByteArray &data)
{
    Q_UNUSED(protocol); /* always zero */

    if (data.size() != 4) {
        return;
    }

    quint32 usec = (quint8)data[0];
    usec = (quint8)data[3];
    usec <<= 8;
    usec |= (quint8)data[2];
    usec <<= 8;
    usec |= (quint8)data[1];
    usec <<= 8;
    usec |= (quint8)data[0];

    if (usec > 60000) {
        return;
    }

    int msec = usec / 1000;

    int value;

    if (msec > (45 - 2))
        value = 9;
    else if (msec > (40 - 2))
        value = 8;
    else if (msec > (35 - 2))
        value = 7;
    else if (msec > (30 - 2))
        value = 6;
    else if (msec > (25 - 2))
        value = 5;
    else if (msec > (20 - 2))
        value = 4;
    else if (msec > (15 - 2))
        value = 3;
    else if (msec > (10 - 2))
        value = 2;
    else if (msec > (5 - 2))
        value = 1;
    else
        return;

    value = value -1;
    waterLevel = value * levelSensorHeight;

    emit waterLevelChanged(waterLevel);

    if ((value == numberOfSensors) && alarmEnabled) {
        if (isConnected && !isAlarm) {
            isAlarm = true;
            emit highWaterLevelAlarm();
        }
    } else if (value < (numberOfSensors - 1)) {
        isAlarm = false;
    }
}

void WaterTower::deviceConnect()
{
    isConnected = true;
}

void WaterTower::deviceDisconnect()
{
    isConnected = false;
}

void WaterTower::trigger()
{
    if (enabled) {
        com->sendRequest(0, QByteArray(1, sampleInterval));
        timer->start(sampleInterval * 1000);
    }
}

void WaterTower::pauseAlarm()
{

}

void WaterTower::stopAlarm()
{

}
