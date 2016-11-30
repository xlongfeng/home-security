#ifndef WATERTOWERWIDGET_H
#define WATERTOWERWIDGET_H

#include <QGroupBox>
#include <QCheckBox>
#include <QSpinBox>

namespace Ui {
class WaterTowerWidget;
}

class WaterTower;

class WaterTowerWidget : public QGroupBox
{
    Q_OBJECT

public:
    QString readableName(int id);

    QWidget *getEnableWidget()
    {
        return qobject_cast<QWidget *>(enableWidget);
    }

    QWidget *getAlarmEnableWidget()
    {
        return qobject_cast<QWidget *>(enableAlarmWidget);
    }

    QWidget *getAddressWidget()
    {
        return qobject_cast<QWidget *>(addressWidget);
    }

    QWidget *getRadiusWidget()
    {
        return qobject_cast<QWidget *>(radiusWidget);
    }

    QWidget *getLevelSensorHeightWidget()
    {
        return qobject_cast<QWidget *>(levelSensorHeightWidget);
    }

    QWidget *getLevelSensorNumberWidget()
    {
        return qobject_cast<QWidget *>(levelSensorNumberWidget);
    }

    static QWidget *getSampleIntervalWidget();
    static WaterTowerWidget *instance(int identity);

signals:
    void layoutChanged();

public slots:
    void sampleIntervalChanged(int value);
    void readyForUse(bool checked);
    void enableAlarm(bool checked);
    void addressChanged(int value);
    void radiusChanged(int value);
    void levelSensorHeightChanged(int value);
    void levelSensorNumberChanged(int value);
    void waterLevelChanged(int centimetre);
    void deviceConnect();
    void deviceDisconnect();
    void highWaterLevelAlarm();

private:
    Q_DISABLE_COPY(WaterTowerWidget)
    explicit WaterTowerWidget(int id, QWidget *parent = 0);


private:
    Ui::WaterTowerWidget *ui;
    WaterTower *waterTower;
    QString uuid;

    QCheckBox *enableWidget;
    QCheckBox *enableAlarmWidget;
    QSpinBox *addressWidget;
    QSpinBox *radiusWidget;
    QSpinBox *levelSensorHeightWidget;
    QSpinBox *levelSensorNumberWidget;
    static QSpinBox *sampleIntervalWidget;

    static QMap<int, WaterTowerWidget*> instanceMap;
};

#endif // WATERTOWERWIDGET_H
