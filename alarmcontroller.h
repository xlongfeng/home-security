#ifndef ALARMCONTROLLER_H
#define ALARMCONTROLLER_H

#include <QObject>

class AlarmController : public QObject
{
    Q_OBJECT
public:
    explicit AlarmController(QObject *parent = 0);

signals:

public slots:
};

#endif // ALARMCONTROLLER_H
