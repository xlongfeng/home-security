#include <QTimer>
#include <QDebug>

#include "watertower.h"

const quint32 AcousticVelocity = 340;

quint32 WaterTower::sampleInterval = 10;

QMap<int, WaterTower*> WaterTower::waterTowerMap;

WaterTower::WaterTower(int id, QObject *parent) :
    QObject(parent),
    identity(id),
    height(200),
    heightReserved(10),
    waterLevel(0),
    alarmAck(false)
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(trigger()));
    timer->setSingleShot(true);
    timer->start(3 * 1000);
}

/* static */
void WaterTower::setSampleInterval(quint32 second)
{
    sampleInterval = second;
}

/* static */
quint32 WaterTower::getSampleInterval()
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

void WaterTower::trigger()
{
    timer->start(sampleInterval * 1000);
}

void WaterTower::readSample(quint32 microsecond)
{
    qint32 distance;
    if (microsecond < 5 || microsecond > 10000)
        return;

    distance = (microsecond / 2) * AcousticVelocity * 100 / 1000000;
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

void WaterTower::pauseAlarm()
{

}

void WaterTower::stopAlarm()
{
    alarmAck = true;
}
