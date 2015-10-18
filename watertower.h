#ifndef WATERTOWER_H
#define WATERTOWER_H

#include <QObject>
#include <QMap>

class QTimer;

class WaterTower : public QObject
{
    Q_OBJECT
public:

    int getIdentity() const
    {
        return identity;
    }

    void setHeight(qint32 centimetre)
    {
        height = centimetre;
    }

    void setHeightReserved(qint32 centimetre)
    {
        heightReserved = centimetre;
    }

    qint32 getHeight() const
    {
        return height;
    }

    qint32 getHeightReserved() const
    {
        return heightReserved;
    }

    qint32 getWaterLevel() const
    {
        return waterLevel;
    }

    static void setSampleInterval(quint32 second);
    static quint32 getSampleInterval();
    static WaterTower *instance(int identity);

signals:
    void waterLevelChanged(quint32 centimetre);
    void highWaterLevelAlarm();

public slots:
    void trigger();
    void readSample(quint32 microsecond);
    void pauseAlarm();
    void stopAlarm();

private:
    Q_DISABLE_COPY(WaterTower)
    explicit WaterTower(int id, QObject *parent = 0);

private:
    int identity;
    bool enabled;
    QTimer *timer;

    /*  measured in the unit of "centimetre"  */
    qint32 height;
    qint32 heightReserved;
    qint32 waterLevel;

    bool alarmAck;

    static quint32 sampleInterval;
    static QMap<int, WaterTower*> waterTowerMap;
};

#endif // WATERTOWER_H