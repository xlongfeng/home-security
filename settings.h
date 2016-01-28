#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QTime>

class Settings : public QSettings
{
    Q_OBJECT

public:
    static Settings *instance();

    void setBrightness(int value);
    int getBrightness();

    void setVolume(int value);
    int getVolume();

    int getIdleTime();
    QTime getIdleTimeFrom() const;
    QTime getIdleTimeTo() const;

signals:
    void volumeChanged(int value);
    void idleTimeChanged(int value);
    void idleTimeFromChanged(const QTime &value);
    void idleTimeToChanged(const QTime &value);

public slots:
    void setIdleTime(int value);
    void setIdleTimeFrom(const QTime &value);
    void setIdleTimeTo(const QTime &value);

private:
    explicit Settings(const QString &fileName, Format format = IniFormat, QObject *parent = 0);
    Q_DISABLE_COPY(Settings)

private:
    static Settings *self;
};

#endif // SETTINGS_H
