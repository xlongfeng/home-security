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
#include <linux/spi/spidev.h>

#include <QFile>
#include <QTime>
#include <QUdpSocket>
#include <QDebug>


#include "multipointcom.h"

// #define DEBUG

static const quint32 MaxTransmitTimeout = 200;
static const quint16 IFFilterTable[][2] = {
    { 322, 0x26 },
    { 3355, 0x88 },
    { 3618, 0x89 },
    { 4202, 0x8A },
    { 4684, 0x8B },
    { 5188, 0x8C },
    { 5770, 0x8D },
    { 6207, 0x8E }
};
static const quint32 MaxFifoLength = 64;

QMutex MultiPointCom::mutex;
bool MultiPointCom::deviceInitialized = false;
QTime MultiPointCom::lastConnectTime = QTime::currentTime();
quint32 MultiPointCom::disconnectCount = 1;
int MultiPointCom::spi = -1;
quint64 MultiPointCom::_freqCarrier = 443000000;
quint8 MultiPointCom::_freqChannel = 0;
quint16 MultiPointCom::_kbps = 100;
quint16 MultiPointCom::_packageSign = 0xDEAD;

static const char *spiDev = "/dev/spidev0.0";
static quint8 mode = SPI_MODE_0;
static quint8 bits = 8;
static quint32 speed = 2000000;
static quint16 delay = 10;

const char *irqGPIO = "/sys/devices/virtual/gpio/gpio134/value";
const char *sdnGPIO = "/sys/devices/virtual/gpio/gpio135/value";

MultiPointCom::MultiPointCom(quint8 id, QObject *parent) :
    QThread(parent),
    identity(id)
{

}

MultiPointCom::~MultiPointCom()
{

}

bool MultiPointCom::sendRequest(char protocol, const QByteArray &data)
{
    if (isRunning())
        return false;

    request.clear();
    request.append(identity);
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
        deviceInitialized = false;
    }

#if __arm__
    if (!deviceInitialized) {
        qDebug() << "Multi point communication initialized";
        deviceInitialized = true;
        init();
        setBaudRate(30);
        setFrequency(433);
        lastConnectTime = QTime::currentTime();
    }

    if (sendPacket(request.size(), (quint8 *)request.data(), true)) {
        quint8 id = response.at(0);
        if (id == identity) {
            lastConnectTime = QTime::currentTime();
            emit deviceConnected();
            emit responseReceived(response.at(1), response.mid(2));
        }
    } else {
        emit deviceDisconnected();
    }
#else
    if (!deviceInitialized) {
        qDebug() << "Multi point communication initialized";
        deviceInitialized = true;
        lastConnectTime = QTime::currentTime();
    }

    QUdpSocket *udp = new QUdpSocket();
    udp->writeDatagram(request, QHostAddress::LocalHost, 19999);

    if (udp->waitForReadyRead(100)) {
        response.resize(udp->pendingDatagramSize());
        udp->readDatagram(response.data(), response.size());
        quint8 id = response.at(0);
        qDebug() << identity << response.toHex();
        if (id == identity) {
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

void MultiPointCom::setFrequency(unsigned long baseFrequencyMhz)
{
    if ((baseFrequencyMhz < 240) || (baseFrequencyMhz > 930))
        return; // invalid frequency

    _freqCarrier = baseFrequencyMhz;
    quint8 highBand = 0;
    if (baseFrequencyMhz >= 480) {
        highBand = 1;
    }

    double fPart = (baseFrequencyMhz / (10 * (highBand + 1))) - 24;

    quint8 freqband = (quint8) fPart; // truncate the int

    quint16 freqcarrier = (fPart - freqband) * 64000;

    // sideband is always on (0x40) :
    quint8 vals[3] = { (quint8)(0x40 | (highBand << 5) | (freqband & 0x3F)), (quint8)(freqcarrier >> 8), (quint8)(freqcarrier & 0xFF) };

    burstWrite(REG_FREQBAND, vals, 3);
}

void MultiPointCom::setCommsSignature(quint16 signature)
{
    _packageSign = signature;

    changeRegister(REG_TRANSMIT_HEADER3, _packageSign >> 8); // header (signature) quint8 3 val
    changeRegister(REG_TRANSMIT_HEADER2, (_packageSign & 0xFF)); // header (signature) quint8 2 val

    changeRegister(REG_CHECK_HEADER3, _packageSign >> 8); // header (signature) quint8 3 val for receive checks
    changeRegister(REG_CHECK_HEADER2, (_packageSign & 0xFF)); // header (signature) quint8 2 val for receive checks

#ifdef DEBUG
    printf("Package signature is set!\n");
#endif
}

void MultiPointCom::init()
{
    if (spi > 0)
        close(spi);
    spi = open(spiDev, O_RDWR);
    ioctl(spi, SPI_IOC_WR_MODE, &mode);
    ioctl(spi, SPI_IOC_WR_BITS_PER_WORD, &bits);
    ioctl(spi, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

    hardReset();
}

void MultiPointCom::boot()
{
    /*
     quint8 currentFix[] = { 0x80, 0x40, 0x7F };
     burstWrite(REG_CHARGEPUMP_OVERRIDE, currentFix, 3); // refer to AN440 for reasons

     changeRegister(REG_GPIO0_CONF, 0x0F); // tx/rx data clk pin
     changeRegister(REG_GPIO1_CONF, 0x00); // POR inverted pin
     changeRegister(REG_GPIO2_CONF, 0x1C); // clear channel pin
     */
    changeRegister(REG_AFC_TIMING_CONTROL, 0x02); // refer to AN440 for reasons
    changeRegister(REG_AFC_LIMITER, 0xFF); // write max value - excel file did that.
    changeRegister(REG_AGC_OVERRIDE, 0x60); // max gain control
    changeRegister(REG_AFC_LOOP_GEARSHIFT_OVERRIDE, 0x3C); // turn off AFC
    changeRegister(REG_DATAACCESS_CONTROL, 0xAD); // enable rx packet handling, enable tx packet handling, enable CRC, use CRC-IBM
    changeRegister(REG_HEADER_CONTROL1, 0x0C); // no broadcast address control, enable check headers for quint8s 3 & 2
    changeRegister(REG_HEADER_CONTROL2, 0x22);  // enable headers quint8 3 & 2, no fixed package length, sync word 3 & 2
    changeRegister(REG_PREAMBLE_LENGTH, 0x08); // 8 * 4 bits = 32 bits (4 quint8s) preamble length
    changeRegister(REG_PREAMBLE_DETECTION, 0x3A); // validate 7 * 4 bits of preamble  in a package
    changeRegister(REG_SYNC_WORD3, 0x2D); // sync quint8 3 val
    changeRegister(REG_SYNC_WORD2, 0xD4); // sync quint8 2 val

    changeRegister(REG_TX_POWER, 0x1F); // max power

    changeRegister(REG_CHANNEL_STEPSIZE, 0x64); // each channel is of 1 Mhz interval

    setFrequency(_freqCarrier); // default freq
    setBaudRate(_kbps); // default baud rate is 100kpbs
    setChannel(_freqChannel); // default channel is 0
    setCommsSignature(_packageSign); // default signature

    switchMode(Ready);
}

bool MultiPointCom::sendPacket(quint8 length, const quint8* data, bool waitResponse, int ackTimeout)
{
    clearTxFIFO();
    changeRegister(REG_PKG_LEN, length);

    burstWrite(REG_FIFO, data, length);

    changeRegister(REG_INT_ENABLE1, 0x04); // set interrupts on for package sent
    changeRegister(REG_INT_ENABLE2, 0x00); // set interrupts off for anything else
    //read interrupt registers to clean them
    readRegister(REG_INT_STATUS1);
    readRegister(REG_INT_STATUS2);

    switchMode(TXMode | Ready);

    QTime timeout = QTime::currentTime().addMSecs(MaxTransmitTimeout);

    while (QTime::currentTime() < timeout) {
#ifdef linux
        if (!isInterrupt()) {
            continue;
        }
#endif

        quint8 intStatus = readRegister(REG_INT_STATUS1);
        readRegister(REG_INT_STATUS2);

        if (intStatus & 0x04) {
            switchMode(Ready | TuneMode);
#ifdef DEBUG
            printf("Package sent! -- %x\n", intStatus);
#endif
            // package sent. now, return true if not to wait ack, or wait ack (wait for packet only for 'remaining' amount of time)
            if (waitResponse) {
                if (waitForPacket(ackTimeout)) {
                    getPacketReceived();
                    return true;
                } else {
                    return false;
                }
            } else {
                return true;
            }
        } else {
            yieldCurrentThread();
        }
    }

    //timeout occured.
//#ifdef DEBUG
    printf("Timeout in Transit -- \n");
//#endif
    emit error(TransmitTimeout);
    switchMode(Ready);

    if (readRegister(REG_DEV_STATUS) & 0x80) {
        clearFIFO();
    }

    return false;
}

bool MultiPointCom::waitForPacket(int waitMs)
{
    startListening();

    QTime timeout = QTime::currentTime().addMSecs(waitMs);

    while (QTime::currentTime() < timeout) {
        if (!isPacketReceived()) {
            yieldCurrentThread();
        } else {
            return true;
        }
    }
    //timeout occured.

    // printf("Timeout in receive-- \n");
    emit error(ReceiveTimeout);

    switchMode(Ready);
    clearRxFIFO();

    return false;
}

void MultiPointCom::getPacketReceived()
{
    quint8 length = readRegister(REG_RECEIVED_LENGTH);

    response.resize(length);

    burstRead(REG_FIFO, (quint8 *)response.data(), length);

    clearRxFIFO(); // which will also clear the interrupts
}

void MultiPointCom::setChannel(quint8 channel)
{
    changeRegister(REG_FREQCHANNEL, channel);
}

void MultiPointCom::switchMode(quint8 mode)
{
    changeRegister(REG_STATE, mode); // receive mode
    //delay(20);
#ifdef DEBUG
    quint8 val = readRegister(REG_DEV_STATUS);
    if (val == 0 || val == 0xFF) {
        printf("%x -- WHAT THE HELL!!\n", val);
    }
#endif
}

void MultiPointCom::changeRegister(Registers reg, quint8 value)
{
    burstWrite(reg, &value, 1);
}

void MultiPointCom::setBaudRate(quint16 kbps)
{
    // chip normally supports very low bps values, but they are cumbersome to implement - so I just didn't implement lower bps values
    if ((kbps > 256) || (kbps < 1))
        return;
    _kbps = kbps;

    quint8 freqDev = kbps <= 10 ? 15 : 150;		// 15khz / 150 khz
    quint8 modulationValue = _kbps < 30 ? 0x4c : 0x0c;		// use FIFO Mode, GFSK, low baud mode on / off

    quint8 modulationVals[] = { modulationValue, 0x23, (quint8)round((freqDev * 1000.0) / 625.0) }; // msb of the kpbs to 3rd bit of register
    burstWrite(REG_MODULATION_MODE1, modulationVals, 3);

    // set data rate
    quint16 bpsRegVal = round((kbps * (kbps < 30 ? 2097152 : 65536.0)) / 1000.0);
    quint8 datarateVals[] = { (quint8)(bpsRegVal >> 8), (quint8)(bpsRegVal & 0xFF) };

    burstWrite(REG_TX_DATARATE1, datarateVals, 2);

    //now set the timings
    quint16 minBandwidth = (2 * (quint32) freqDev) + kbps;
#ifdef DEBUG
    printf("min Bandwidth value: %x\n", minBandwidth);
#endif
    quint8 IFValue = 0xff;
    //since the table is ordered (from low to high), just find the 'minimum bandwith which is greater than required'
    for (quint8 i = 0; i < 8; ++i) {
        if (IFFilterTable[i][0] >= (minBandwidth * 10)) {
            IFValue = IFFilterTable[i][1];
            break;
        }
    }
#ifdef DEBUG
    printf("Selected IF value: %x\n", IFValue);
#endif

    changeRegister(REG_IF_FILTER_BW, IFValue);

    quint8 dwn3_bypass = (IFValue & 0x80) ? 1 : 0; // if msb is set
    quint8 ndec_exp = (IFValue >> 4) & 0x07; // only 3 bits

    quint16 rxOversampling = round((500.0 * (1 + 2 * dwn3_bypass)) / ((pow(2.0, ndec_exp - 3)) * (double ) kbps));

    quint32 ncOffset = ceil(((double) kbps * (pow(2.0, ndec_exp + 20))) / (500.0 * (1 + 2 * dwn3_bypass)));

    quint16 crGain = 2 + ((65535 * (qint64) kbps) / ((qint64) rxOversampling * freqDev));
    quint8 crMultiplier = 0x00;
    if (crGain > 0x7FF) {
        crGain = 0x7FF;
    }
#ifdef DEBUG
    printf("dwn3_bypass value: %x\n", dwn3_bypass);
    printf("ndec_exp value: %x\n", ndec_exp);
    printf("rxOversampling value: %x\n", rxOversampling);
    printf("ncOffset value: %x\n", ncOffset);
    printf("crGain value: %x\n", crGain);
    printf("crMultiplier value: %x\n", crMultiplier);
#endif

    quint8 timingVals[] = { (quint8)(rxOversampling & 0x00FF), (quint8)(((rxOversampling & 0x0700) >> 3) | ((ncOffset >> 16) & 0x0F)),
            (quint8)((ncOffset >> 8) & 0xFF), (quint8)(ncOffset & 0xFF), (quint8)(((crGain & 0x0700) >> 8) | crMultiplier), (quint8)(crGain & 0xFF) };

    burstWrite(REG_CLOCK_RECOVERY_OVERSAMPLING, timingVals, 6);
}

quint8 MultiPointCom::readRegister(Registers reg)
{
    quint8 val = 0xFF;
    burstRead(reg, &val, 1);
    return val;
}

void MultiPointCom::burstWrite(Registers startReg, const quint8 value[], quint8 length)
{
    QByteArray txBuf;
    txBuf.append((quint8) startReg | 0x80); // set MSB
    txBuf.append((const char *)value, length);

    spi_ioc_transfer tr;
    memset(&tr, 0, sizeof(spi_ioc_transfer));

    tr.tx_buf = (unsigned long)txBuf.data();
    tr.rx_buf = 0;
    tr.len = txBuf.size();
    tr.delay_usecs = delay;
    tr.speed_hz = speed;
    tr.bits_per_word = bits;

    ioctl(spi, SPI_IOC_MESSAGE(1), &tr);
}

void MultiPointCom::burstRead(Registers startReg, quint8 value[], quint8 length)
{
    QByteArray txBuf(length + 1, 0xFF);
    QByteArray rxBuf(length + 1, 0x0);
    txBuf[0] = ((quint8) startReg & 0x7F); // clr MSB

    spi_ioc_transfer tr;
    memset(&tr, 0, sizeof(spi_ioc_transfer));

    tr.tx_buf = (unsigned long)txBuf.data();
    tr.rx_buf = (unsigned long)rxBuf.data();
    tr.len = txBuf.size();
    tr.delay_usecs = delay;
    tr.speed_hz = speed;
    tr.bits_per_word = bits;

    ioctl(spi, SPI_IOC_MESSAGE(1), &tr);

    memcpy(value, rxBuf.data() + 1, length);
}

void MultiPointCom::readAll()
{
    quint8 allValues[0x7F];

    burstRead(REG_DEV_TYPE, allValues, 0x7F);

    for (quint8 i = 0; i < 0x7f; ++i) {
        printf("REG(%x) : %x\n", (int) REG_DEV_TYPE + i, (int) allValues[i]);
    }
}

void MultiPointCom::clearTxFIFO()
{
    changeRegister(REG_OPERATION_CONTROL, 0x01);
    changeRegister(REG_OPERATION_CONTROL, 0x00);
}

void MultiPointCom::clearRxFIFO()
{
    changeRegister(REG_OPERATION_CONTROL, 0x02);
    changeRegister(REG_OPERATION_CONTROL, 0x00);
}

void MultiPointCom::clearFIFO()
{
    changeRegister(REG_OPERATION_CONTROL, 0x03);
    changeRegister(REG_OPERATION_CONTROL, 0x00);
}

void MultiPointCom::softReset()
{
    changeRegister(REG_STATE, 0x80);

    quint8 reg = readRegister(REG_INT_STATUS2);
    while ((reg & 0x02) != 0x02) {
        msleep(1);
        reg = readRegister(REG_INT_STATUS2);
    }

    boot();
}

void MultiPointCom::hardReset()
{
    turnOff();
    turnOn();

    QTime timeout = QTime::currentTime().addMSecs(1000);
    quint8 reg;

    do {
        reg = readRegister(REG_INT_STATUS2);
        if ((reg & 0x02) == 0x02)
            break;
        /* printf("POR: %x\n", reg); */

        if (timeout < QTime::currentTime()) {
            emit error(DeviceBroken);
            return;
        }
    } while (1);

#ifdef DEBUG
    printf("POR: %x\n", reg);
#endif

    boot();
}

void MultiPointCom::startListening()
{
    clearRxFIFO(); // clear first, so it doesn't overflow if packet is big

    changeRegister(REG_INT_ENABLE1, 0x03); // set interrupts on for package received and CRC error

#ifdef DEBUG
    changeRegister(REG_INT_ENABLE2, 0xC0);
#else
    changeRegister(REG_INT_ENABLE2, 0x00); // set other interrupts off
#endif
    //read interrupt registers to clean them
    readRegister(REG_INT_STATUS1);
    readRegister(REG_INT_STATUS2);

    switchMode(RXMode | Ready);
}

bool MultiPointCom::isPacketReceived()
{
#ifdef linux
    if (!isInterrupt()) {
        return false; // if no interrupt occured, no packet received is assumed (since startListening will be called prior, this assumption is enough)
    }
#endif
    // check for package received status interrupt register
    quint8 intStat = readRegister(REG_INT_STATUS1);

#ifdef DEBUG
    quint8 intStat2 = readRegister(REG_INT_STATUS2);

    if (intStat2 & 0x40) { //interrupt occured, check it && read the Interrupt Status1 register for 'preamble '

        printf("HEY!! HEY!! Valid Preamble detected -- %x\n", intStat2);

    }

    if (intStat2 & 0x80) { //interrupt occured, check it && read the Interrupt Status1 register for 'preamble '

        printf("HEY!! HEY!! SYNC WORD detected -- %x\n", intStat2);

    }
#else
    readRegister(REG_INT_STATUS2);
#endif

    if (intStat & 0x02) { //interrupt occured, check it && read the Interrupt Status1 register for 'valid packet'
        switchMode(Ready | TuneMode); // if packet came, get out of Rx mode till the packet is read out. Keep PLL on for fast reaction
#ifdef DEBUG
        printf("Packet detected -- %x\n", intStat);
#endif
        return true;
    } else if (intStat & 0x01) { // packet crc error
        switchMode(Ready); // get out of Rx mode till buffers are cleared
//#ifdef DEBUG
        printf("CRC Error in Packet detected!-- %x\n", intStat);
//#endif
        clearRxFIFO();
        switchMode(RXMode | Ready); // get back to work
        return false;
    }

    //no relevant interrupt? no packet!

    return false;
}

void MultiPointCom::turnOn()
{
#ifdef linux
    puts(sdnGPIO, "0");
#endif
    msleep(20);
}

void MultiPointCom::turnOff()
{
#ifdef linux
    puts(sdnGPIO, "1");
#endif
    msleep(1);
}

bool MultiPointCom::isInterrupt()
{
    char value[1];
    bool interrupted = false;

    if (gets(irqGPIO, value, 1) == 1) {
        if (value[0] == '0')
            interrupted = true;
    }

    return interrupted;
}

int MultiPointCom::puts(const char *filename, const char *str)
{
    QFile file(filename);
    int ret = -1;

    if (file.open(QIODevice::WriteOnly | QIODevice::Unbuffered)) {
        ret = file.write(str);
        file.close();
    }
    return ret;
}

int MultiPointCom::gets(const char *filename, char *str, int len)
{
    QFile file(filename);
    int ret = -1;

    if (file.open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
        ret = file.read(str, len);
        file.close();
    }
    return ret;
}
