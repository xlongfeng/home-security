#include <QKeyEvent>
#include <QTimer>
#include <QDebug>

#include "settings.h"
#include "hal.h"
#include "keypresseater.h"

KeyPressEater::KeyPressEater(QObject *parent) :
    QObject(parent)
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(ticktack()));
    idleTime = Settings::instance()->getIdleTime();
    timer->start(idleTime * 60 * 1000);
}

bool KeyPressEater::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type()) {
    case QEvent::KeyPress:
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (!Hal::instance()->isPowerOn()) {
            if (keyEvent->key() != Qt::Key_PowerOff)
                    return true;
        }
    }
    case QEvent::MouseMove:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonDblClick:
    case QEvent::Wheel:
        emit mouseActive();
        timer->start(idleTime * 60 * 1000);
        break;
    default:
        break;
    }

    return QObject::eventFilter(obj, event);
}

void KeyPressEater::setIdleTime(int value)
{
    idleTime = value;
}

void KeyPressEater::ticktack()
{
    if (!Hal::instance()->isPowerOn())
        return;

    const QTime &currentTime = QTime::currentTime();
    const QTime &from = Settings::instance()->getIdleTimeFrom();
    const QTime &to = Settings::instance()->getIdleTimeTo();

    if (to < from) {
        if (((currentTime > from) && (currentTime < QTime(23, 59, 0))) ||
            ((currentTime > QTime(0, 0, 0)) && (currentTime < to)))
            Hal::instance()->powerOff();
    } else {
        if ((currentTime > from) && (currentTime < to))
            Hal::instance()->powerOff();
    }
}
