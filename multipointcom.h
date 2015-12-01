#ifndef MULTIPOINTCOM_H
#define MULTIPOINTCOM_H

#include <QThread>
#include <QMutex>

class MultiPointCom : public QThread
{
    Q_OBJECT

public:
    enum Error {
        NoError,
        DeviceBroken,
        TransmitTimeout,
        ReceiveTimeout
    };

    MultiPointCom(QObject *parent = 0);
    ~MultiPointCom();

    void setAddress(quint8 addr)
    {
        address = addr;
    }

    bool sendRequest(char protocol, const QByteArray &data);

signals:
    void responseReceived(char protocol, const QByteArray &data);
    void deviceConnected();
    void deviceDisconnected();
    void error(Error error);

protected:
    virtual void run();

private:
    quint8 address;
    QByteArray request;
    QByteArray response;
    static QMutex mutex;
    static bool deviceInitialized;
    static QTime lastConnectTime;
    static quint32 disconnectCount;
    static int device;
};

#endif // MULTIPOINTCOM_H
