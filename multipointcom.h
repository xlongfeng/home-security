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

    MultiPointCom(quint8 id, QObject *parent = 0);
    ~MultiPointCom();

    bool sendRequest(char protocol, const QByteArray &data);

signals:
    void responseReceived(char protocol, const QByteArray &data);
    void connected();
    void disconnected();
    void error(Error error);

protected:
    virtual void run();

private:
    // sets the freq. call before boot
    void setFrequency(unsigned long baseFrequency);
    // sets the channel. call before switching to tx or rx mode
    void setChannel(quint8 channel);
    // sets the  bps. call before switching to tx or rx mode - min:1, max: 256
    void setBaudRate(quint16 kbps);
    void init();
    // sets SPI and pins ready and boot the radio
    void boot();
    // used to 'sign' packets with a predetermined signature - call before boot
    void setCommsSignature(quint16 signature);

    // switches to Tx mode and sends the package, then optionally receives response package
    bool sendPacket(quint8 length, const quint8* data, bool waitResponse = false, int ackTimeout = 100);

    // switch to Rx mode (don't block)
    void startListening();

    // check for the packet received flags
    bool isPacketReceived();

    // switch to Rx mode and wait until timeout or 'valid' package to arrive
    bool waitForPacket(int waitMs);
    // read from FIFO
    void getPacketReceived();

    void readAll();

    void clearTxFIFO();
    void clearRxFIFO();

    void clearFIFO();

    void softReset();

    void hardReset();

    void turnOn();
    void turnOff();
    bool isInterrupt();

    void switchMode(quint8 mode);

private:
    enum AntennaMode {
        RXMode = 0x04, TXMode = 0x08, Ready = 0x01, TuneMode = 0x02
    };

    enum Registers {
        REG_DEV_TYPE = 0x00,
        REG_DEV_VERSION = 0x01,
        REG_DEV_STATUS = 0x02,

        REG_INT_STATUS1 = 0x03,
        REG_INT_STATUS2 = 0x04,
        REG_INT_ENABLE1 = 0x05,
        REG_INT_ENABLE2 = 0x06,
        REG_STATE = 0x07,
        REG_OPERATION_CONTROL = 0x08,

        REG_GPIO0_CONF = 0x0B,
        REG_GPIO1_CONF = 0x0C,
        REG_GPIO2_CONF = 0x0D,
        REG_IOPORT_CONF = 0x0E,

        REG_IF_FILTER_BW = 0x1C,
        REG_AFC_LOOP_GEARSHIFT_OVERRIDE = 0x1D,
        REG_AFC_TIMING_CONTROL = 0x1E,
        REG_CLOCK_RECOVERY_GEARSHIFT = 0x1F,
        REG_CLOCK_RECOVERY_OVERSAMPLING = 0x20,
        REG_CLOCK_RECOVERY_OFFSET2 = 0x21,
        REG_CLOCK_RECOVERY_OFFSET1 = 0x22,
        REG_CLOCK_RECOVERY_OFFSET0 = 0x23,
        REG_CLOCK_RECOVERY_TIMING_GAIN1 = 0x24,
        REG_CLOCK_RECOVERY_TIMING_GAIN0 = 0x25,
        REG_RSSI = 0x26,
        REG_RSSI_THRESHOLD = 0x27,

        REG_AFC_LIMITER = 0x2A,
        REG_AFC_CORRECTION_READ = 0x2B,

        REG_DATAACCESS_CONTROL = 0x30,
        REG_EZMAC_STATUS = 0x31,
        REG_HEADER_CONTROL1 = 0x32,
        REG_HEADER_CONTROL2 = 0x33,
        REG_PREAMBLE_LENGTH = 0x34,
        REG_PREAMBLE_DETECTION = 0x35,
        REG_SYNC_WORD3 = 0x36,
        REG_SYNC_WORD2 = 0x37,
        REG_SYNC_WORD1 = 0x38,
        REG_SYNC_WORD0 = 0x39,
        REG_TRANSMIT_HEADER3 = 0x3A,
        REG_TRANSMIT_HEADER2 = 0x3B,
        REG_TRANSMIT_HEADER1 = 0x3C,
        REG_TRANSMIT_HEADER0 = 0x3D,

        REG_PKG_LEN = 0x3E,

        REG_CHECK_HEADER3 = 0x3F,
        REG_CHECK_HEADER2 = 0x40,
        REG_CHECK_HEADER1 = 0x41,
        REG_CHECK_HEADER0 = 0x42,

        REG_RECEIVED_HEADER3 = 0x47,
        REG_RECEIVED_HEADER2 = 0x48,
        REG_RECEIVED_HEADER1 = 0x49,
        REG_RECEIVED_HEADER0 = 0x4A,

        REG_RECEIVED_LENGTH = 0x4B,

        REG_CHARGEPUMP_OVERRIDE = 0x58,
        REG_DIVIDER_CURRENT_TRIM = 0x59,
        REG_VCO_CURRENT_TRIM = 0x5A,

        REG_AGC_OVERRIDE = 0x69,

        REG_TX_POWER = 0x6D,
        REG_TX_DATARATE1 = 0x6E,
        REG_TX_DATARATE0 = 0x6F,

        REG_MODULATION_MODE1 = 0x70,
        REG_MODULATION_MODE2 = 0x71,

        REG_FREQ_DEVIATION = 0x72,
        REG_FREQ_OFFSET1 = 0x73,
        REG_FREQ_OFFSET2 = 0x74,
        REG_FREQBAND = 0x75,
        REG_FREQCARRIER_H = 0x76,
        REG_FREQCARRIER_L = 0x77,

        REG_FREQCHANNEL = 0x79,
        REG_CHANNEL_STEPSIZE = 0x7A,

        REG_FIFO = 0x7F,
    };

    void changeRegister(Registers reg, quint8 value);
    quint8 readRegister(Registers reg);

    void burstWrite(Registers startReg, const quint8 value[], quint8 length);
    void burstRead(Registers startReg, quint8 value[], quint8 length);

    int puts(const char *filename, const char *str);
    int gets(const char *filename, char *str, int len);

private:
    quint8 identity;
    QByteArray request;
    QByteArray response;
    static QMutex mutex;
    static bool deviceInitialized;
    static QTime lastConnectTime;
    static int spi;
    static quint64 _freqCarrier;
    static quint8 _freqChannel;
    static quint16 _kbps;
    static quint16 _packageSign;
};

#endif // MULTIPOINTCOM_H
