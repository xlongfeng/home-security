/*
 * Base on SI4432 library for Arduino - v0.1
 * Modify by xlongfeng <xlongfeng@126.com> 2015
 *
 * Please note that Library uses standart SS pin for NSEL pin on the chip. This is 53 for Mega, 10 for Uno.
 * NOTES:
 *
 * V0.1
 * * Library supports no `custom' changes and usages of GPIO pin. Modify/add/remove your changes if necessary
 * * Radio use variable packet field format with 4 byte address header, first data field as length. Change if necessary
 *
 * made by Ahmet (theGanymedes) Ipkin
 *
 * 2014
 *
 */

#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>

#include <QFile>
#include <QTime>
#include <QUdpSocket>
#include <QDebug>

#include "multipointcom.h"


struct si4432_ioc_transfer {
    __u64		tx_buf;
    __u64		rx_buf;
    __u32		len;
};

/* IOCTL commands */

#define SI4432_IOC_MAGIC			's'

/* not all platforms use <asm-generic/ioctl.h> or _IOC_TYPECHECK() ... */
#define SI4432_MSGSIZE(N) \
    ((((N)*(sizeof (struct si4432_ioc_transfer))) < (1 << _IOC_SIZEBITS)) \
        ? ((N)*(sizeof (struct si4432_ioc_transfer))) : 0)
#define SI4432_IOC_MESSAGE(N) _IOW(SI4432_IOC_MAGIC, 0, char[SI4432_MSGSIZE(N)])

#define SI4432_IOC_RESET    _IOR(SI4432_IOC_MAGIC, 1, __u8)

QMutex MultiPointCom::mutex;
bool MultiPointCom::deviceInitialized = false;
QTime MultiPointCom::lastConnectTime = QTime::currentTime();
quint32 MultiPointCom::disconnectCount = 1;
int MultiPointCom::device = -1;

static const char *si4432Dev = "/dev/si4432";

const char *irqGPIO = "/sys/devices/virtual/gpio/gpio134/value";
const char *sdnGPIO = "/sys/devices/virtual/gpio/gpio135/value";

MultiPointCom::MultiPointCom(QObject *parent) :
    QThread(parent),
    address(0x7F)
{

    if (!deviceInitialized) {
        qDebug() << "Multi point communication initialized";
        deviceInitialized = true;
#ifdef __arm__
        if (device > 0)
            close(device);
        device = open(si4432Dev, O_RDWR);
#endif
        response.reserve(64);
        lastConnectTime = QTime::currentTime();

    }
}

MultiPointCom::~MultiPointCom()
{

}

bool MultiPointCom::sendRequest(char protocol, const QByteArray &data)
{
    if (isRunning())
        return false;

    request.clear();
    request.append(address);
    request.append(protocol);
    request.append(data);

    start();

    return true;
}

void MultiPointCom::run()
{
    mutex.lock();

    QTime timeout = lastConnectTime.addSecs(30);

    if (timeout < QTime::currentTime()) {
        qDebug() << "Device connected timeout" << disconnectCount++;
        lastConnectTime = QTime::currentTime();
#ifdef __arm__
        ioctl(device, SI4432_IOC_RESET, 1);
#endif
    }

#ifdef __arm__
    si4432_ioc_transfer tr;
    char txBuf[64], rxBuf[64];
    memcpy(txBuf, request.data(), request.size());
    memset(&tr, 0, sizeof(si4432_ioc_transfer));
    tr.tx_buf = (__u64)txBuf;
    tr.rx_buf = (__u64)rxBuf;
    tr.len = request.size();
    int len = ioctl(device, SI4432_IOC_MESSAGE(1), &tr);
    if (len > 0) {
        response.clear();
        response.append(rxBuf, len);
        quint8 addr = response.at(0);
        if (addr == address) {
            lastConnectTime = QTime::currentTime();
            emit deviceConnected();
            emit responseReceived(response.at(1), QByteArray(response.data() + 2, len - 2));
        }
    } else {
        emit deviceDisconnected();
    }
#else
    QUdpSocket *udp = new QUdpSocket();
    udp->writeDatagram(request, QHostAddress::LocalHost, 19999);

    if (udp->waitForReadyRead(100)) {
        response.resize(udp->pendingDatagramSize());
        udp->readDatagram(response.data(), response.size());
        quint8 addr = response.at(0);
        if (addr == address) {
            lastConnectTime = QTime::currentTime();
            emit deviceConnected();
            emit responseReceived(response.at(1), response.mid(2));
        }
    } else {
        emit deviceDisconnected();
    }
    delete udp;
#endif

    mutex.unlock();
}
