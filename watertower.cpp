#include <QTimer>
#include <QDebug>

#include "multipointcom.h"
#include "watertower.h"

const quint8 WaterTowerIdentityBase = 0x10;
const quint32 AcousticVelocity = 340;

quint8 WaterTower::sampleInterval = 10;

QMap<int, WaterTower*> WaterTower::waterTowerMap;

WaterTower::WaterTower(quint8 id, QObject *parent) :
    QObject(parent),
    identity(id),
    enabled(false),
    com(new MultiPointCom(WaterTowerIdentityBase + id)),
    height(200),
    heightReserved(10),
    waterLevel(0),
    alarmAck(false)
{
    connect(com, SIGNAL(responseReceived(char,QByteArray)), this, SLOT(responseReceived(char,QByteArray)));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(trigger()));
    timer->setSingleShot(true);
    timer->start(3 * 1000);
}

/* static */
void WaterTower::setSampleInterval(quint8 second)
{
    sampleInterval = second;
}

/* static */
quint8 WaterTower::getSampleInterval()
{
    return sampleInterval;
}

/* static */
WaterTower *WaterTower::instance(int identity)
{
    WaterTower *wt;

    if (waterTowerMap.contains(identity)) {
        wt = waterTowerMap.value(identity);
    } else {
        wt = new WaterTower(identity);
        waterTowerMap[identity] = wt;
    }
    return wt;
}

void WaterTower::responseReceived(char protocol, const QByteArray &data)
{
    if (data.size() != 4) {
        return;
    }

    readSample((data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0]);
}

void WaterTower::trigger()
{
    com->sendRequest(0, QByteArray(1, sampleInterval));
    timer->start(sampleInterval * 1000);
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
    qDebug() << "water tower read sample:" << microsecond;
    if (microsecond < 5 || microsecond > 10000)
        return;

    qint32 distance = (microsecond / 2) * AcousticVelocity * 100 / 1000000;
    waterLevel = height - distance;
    if (waterLevel < 0)
        waterLevel = 0;
    if (waterLevel > height)
        waterLevel = height;
    emit waterLevelChanged(waterLevel);
    if (distance < heightReserved) {
        if (!alarmAck)
            emit highWaterLevelAlarm();
    } else {
        alarmAck = false;
    }
}
