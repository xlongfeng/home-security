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

    void setHeight(int centimetre);
    int getHeight();
    void setHeightReserved(int centimetre);
    int getHeightReserved();

    int waterLevelMinimum() const
    {
        return 0;
    }

    int waterLevelMaxminum() const
    {
        return virtualHeight;
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
    void waterLevelChanged(int centimetre, int rssi);
    void highWaterLevelAlarm();

public slots:
    void responseReceived(char protocol, const QByteArray &data, int rssi);
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
    /*  measured in the unit of "centimetre"  */
    int height;
    int heightReserved;
    int virtualHeight;
    int waterLevel;

    bool isConnected;
    bool isAlarm;

    static quint8 sampleInterval;
    static QMap<int, WaterTower*> instanceMap;
};

#endif // WATERTOWER_H
