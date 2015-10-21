#ifndef WATERTOWER_H
#define WATERTOWER_H

#include <QObject>
#include <QMap>

class QTimer;
class MultiPointCom;

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

    static void setSampleInterval(quint8 second);
    static quint8 getSampleInterval();
    static WaterTower *instance(int identity);

signals:
    void waterLevelChanged(quint32 centimetre);
    void highWaterLevelAlarm();

public slots:
    void responseReceived(char protocol, const QByteArray &data);
    void trigger();
    void pauseAlarm();
    void stopAlarm();

private:
    Q_DISABLE_COPY(WaterTower)
    explicit WaterTower(quint8 id, QObject *parent = 0);

    void readSample(quint32 microsecond);

private:
    quint8 identity;
    bool enabled;
    QTimer *timer;

    MultiPointCom *com;

    /*  measured in the unit of "centimetre"  */
    qint32 height;
    qint32 heightReserved;
    qint32 waterLevel;

    bool alarmAck;

    static quint8 sampleInterval;
    static QMap<int, WaterTower*> waterTowerMap;
};

#endif // WATERTOWER_H
