#ifdef __arm__
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/watchdog.h>
#endif

#include <QTimer>
#include <QDebug>

#include "watchdog.h"

Watchdog *Watchdog::self = 0;

Watchdog::Watchdog(QObject *parent) :
    QObject(parent)
{
#ifdef __arm__
    fd = open("/dev/watchdog", O_WRONLY);

    if (fd == -1) {
        qDebug() << "Watchdog device not enabled";
    }

    int flags = WDIOS_ENABLECARD;
    ioctl(fd, WDIOC_SETOPTIONS, &flags);
#endif

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(keepAlive()));
    timer->start(1000);
}

Watchdog *Watchdog::instance()
{
    if (!self)
        self = new Watchdog();
    return self;
}

void Watchdog::keepAlive()
{
#ifdef __arm__
    int dummy;
    ioctl(fd, WDIOC_KEEPALIVE, &dummy);
#endif
}
