#ifndef KEYPRESSEATER_H
#define KEYPRESSEATER_H

#include <QObject>

class QTimer;

class KeyPressEater : public QObject
{
    Q_OBJECT
public:
    explicit KeyPressEater(QObject *parent = 0);

signals:
    void mouseActive();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void setIdleTime(int value);
    void ticktack();

private:
    QTimer *timer;
    int idleTime;
};

#endif // KEYPRESSEATER_H
