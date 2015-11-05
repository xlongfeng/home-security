#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings : public QSettings
{
    Q_OBJECT
public:
    explicit Settings(const QString &fileName, Format format = IniFormat, QObject *parent = 0);

    static Settings *instance();

signals:

public slots:

private:
    Q_DISABLE_COPY(Settings)

private:
    static Settings *self;
};

#endif // SETTINGS_H
