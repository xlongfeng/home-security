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
    com(new MultiPointCom(WaterTowerIdentityBase + id)),
    enabled(false),
    height(200),
    heightReserved(10),
    waterLevel(0),
    alarmAck(false)
{
    connect(com, SIGNAL(responseReceived(char,QByteArray)), this, SLOT(responseReceived(char,QByteArray)));

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

    readSample((data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0]);
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
    alarmAck = true;
}

void WaterTower::readSample(quint32 microsecond)
{
    qDebug() << identity << "water tower read sample:" << microsecond;
    if (microsecond < 5 || microsecond > 10000)
        return;

    qint32 distance = (microsecond / 2) * AcousticVelocity * 100 / 1000000;
    waterLevel = height - distance;
    if (waterLevel < 0)
        waterLevel = 0;
    if (waterLevel > height)
        waterLevel = height;
    int progress = waterLevel * 100 / height;
    emit waterLevelChanged(waterLevel, progress);
    if (distance < heightReserved) {
        if (!alarmAck)
            emit highWaterLevelAlarm();
    } else {
        alarmAck = false;
    }
}
