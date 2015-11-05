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

    bool isEnabled() const;
    void setEnable(bool enable);

    void setHeight(qint32 centimetre);
    qint32 getHeight();
    void setHeightReserved(qint32 centimetre);
    qint32 getHeightReserved();

    qint32 getWaterLevel() const
    {
        return waterLevel;
    }

    static void setSampleInterval(quint8 second);
    static quint8 getSampleInterval();
    static WaterTower *instance(int identity);
    static const int MaxQuantity;

signals:
    void waterLevelChanged(quint32 centimetre, int progress);
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
    QTimer *timer;

    MultiPointCom *com;

    bool enabled;
    /*  measured in the unit of "centimetre"  */
    qint32 height;
    qint32 heightReserved;
    qint32 waterLevel;

    bool alarmAck;

    static quint8 sampleInterval;
    static QMap<int, WaterTower*> instanceMap;
};

#endif // WATERTOWER_H
