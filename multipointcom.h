#ifndef MULTIPOINTCOM_H
#define MULTIPOINTCOM_H

#include <QObject>
#include <QMap>

class MultiPointCom : public QObject
{
    Q_OBJECT
public:
    explicit MultiPointCom(QObject *parent = 0);

    void send(int id, const QByteArray &data);

signals:
    void receive(int id, const QByteArray &data);

public slots:

private:
    void transmitToRemote();

private:
    QMap<int, QByteArray> cache;
    bool idle;
};

#endif // MULTIPOINTCOM_H
