#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <QObject>

class QTimer;

class Watchdog : public QObject
{
    Q_OBJECT

public:
    static Watchdog *instance();

public slots:
    void keepAlive();

private:
    explicit Watchdog(QObject *parent = 0);
    Q_DISABLE_COPY(Watchdog)

private:
    static Watchdog *self;

    int fd;
    QTimer *timer;
};

#endif // WATCHDOG_H
