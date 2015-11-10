#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings : public QSettings
{
    Q_OBJECT

public:
    static Settings *instance();

    void setBrightness(int value);
    int getBrightness();

    void setVolume(int value);
    int getVolume();

signals:
    void volumeChanged(int value);

public slots:

private:
    explicit Settings(const QString &fileName, Format format = IniFormat, QObject *parent = 0);
    Q_DISABLE_COPY(Settings)

private:
    static Settings *self;
};

#endif // SETTINGS_H
