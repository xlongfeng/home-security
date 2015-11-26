#include <QTimer>
#include <QDebug>

#include "multipointcom.h"
#include "settings.h"
#include "watertower.h"


const quint8 WaterTowerIdentityBase = 0x10;
const quint32 AcousticVelocity = 340;

const int WaterTower::MaxQuantity = 6;
quint8 WaterTower::sampleInterval = 10;

QMap<int, WaterTower*> WaterTower::instanceMap;

WaterTower::WaterTower(quint8 id, QObject *parent) :
    QObject(parent),
    identity(id),
    com(new MultiPointCom()),
    enabled(false),
    height(200),
    heightReserved(10),
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
    getHeight();
    getHeightReserved();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(trigger()));
    timer->setSingleShot(true);

    enabled = isEnabled();
    if (enabled) {
        timer->start(3 * 1000);
    }
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
        Settings::instance()->setValue("enable", enabled);
        Settings::instance()->endGroup();
    } else {
        timer->stop();
    }
}

void WaterTower::setHeight(qint32 centimetre)
{
    height = centimetre;
    Settings::instance()->beginGroup(QString("WaterTower-%1").arg(identity));
    Settings::instance()->setValue("height", height);
    Settings::instance()->endGroup();
}

qint32 WaterTower::getHeight()
{
    Settings::instance()->beginGroup(QString("WaterTower-%1").arg(identity));
    height = Settings::instance()->value("height", 200).toUInt();
    Settings::instance()->endGroup();
    return height;
}

void WaterTower::setHeightReserved(qint32 centimetre)
{
    heightReserved = centimetre;
    Settings::instance()->beginGroup(QString("WaterTower-%1").arg(identity));
    Settings::instance()->setValue("height-reserved", heightReserved);
    Settings::instance()->endGroup();
}

qint32 WaterTower::getHeightReserved()
{
    Settings::instance()->beginGroup(QString("WaterTower-%1").arg(identity));
    heightReserved = Settings::instance()->value("height-reserved", 10).toUInt();
    Settings::instance()->endGroup();
    return heightReserved;
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

    quint32 microsecond = (quint8)data[0];
    microsecond = (quint8)data[3];
    microsecond <<= 8;
    microsecond |= (quint8)data[2];
    microsecond <<= 8;
    microsecond |= (quint8)data[1];
    microsecond <<= 8;
    microsecond |= (quint8)data[0];

    readSample(microsecond);
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

void WaterTower::readSample(quint32 microsecond)
{
    if (microsecond > 60000)
        microsecond = 60000;

    qint32 distance = (microsecond * AcousticVelocity) / (10000 * 2);
    waterLevel = height - distance;

    if (waterLevel < 0)
        waterLevel = 0;
    if (waterLevel > height)
        waterLevel = height;
    int progress = waterLevel * 100 / (height - heightReserved);
    progress = progress < 100 ? progress : 100;

    emit waterLevelChanged(waterLevel, progress);
    if (distance < heightReserved) {
        if (isConnected && !isAlarm) {
            isAlarm = true;
            emit highWaterLevelAlarm();
        }
    } else {
        isAlarm = false;
    }
}
