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

    quint8 getAddress();
    void setAddress(quint8 address);

    bool isEnabled() const;
    void setEnable(bool enable);

    bool isAlarmEnabled() const;
    void setAlarmEnable(bool enable);

    void setRadius(int centimetre);
    int getRadius();

    void setLevelSensorHeight(int centimetre);
    int getLevelSensorHeight();
    void setSensorNumber(int centimetre);
    int getSensorNumber();

    int getHeight() const
    {
        return height;
    }

    int getWaterLevel() const
    {
        return waterLevel;
    }

    static void setSampleInterval(quint8 second);
    static quint8 getSampleInterval();
    static WaterTower *instance(int identity);
    static const int MaxQuantity;

signals:
    void deviceConnected();
    void deviceDisconnected();
    void waterLevelRangeChanged(int minimum, int maximum);
    void waterLevelChanged(int centimetre);
    void highWaterLevelAlarm();

public slots:
    void responseReceived(char protocol, const QByteArray &data);
    void deviceConnect();
    void deviceDisconnect();
    void trigger();
    void pauseAlarm();
    void stopAlarm();

private:
    Q_DISABLE_COPY(WaterTower)
    explicit WaterTower(quint8 id, QObject *parent = 0);

private:
    quint8 identity;
    QTimer *timer;

    MultiPointCom *com;

    bool enabled;
    bool alarmEnabled;

    int levelSensorHeight;  /*  measured in the unit of "centimetre"  */
    int numberOfSensors;

    int height;             /*  watertower height, measured in the unit of "centimetre"  */
    int waterLevel;         /*  measured in the unit of "centimetre"  */

    bool isConnected;
    bool isAlarm;

    static quint8 sampleInterval;
    static QMap<int, WaterTower*> instanceMap;
};

#endif // WATERTOWER_H
