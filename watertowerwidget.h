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

    QWidget *getBarrelHeightWidget()
    {
        return qobject_cast<QWidget *>(barrelHeightWidget);
    }

    QWidget *getReservedHeightWidget()
    {
        return qobject_cast<QWidget *>(reservedHeightWidget);
    }

    static QWidget *getSampleIntervalWidget();
    static WaterTowerWidget *instance(int identity);

signals:
    void layoutChanged();

public slots:
    void sampleIntervalChanged(int value);
    void readyForUse(bool checked);
    void barrelHeightChanged(int value);
    void reservedHeightChanged(int value);
    void waterLevelChanged(quint32 centimetre, int progress);
    void highWaterLevelAlarm();

private:
    Q_DISABLE_COPY(WaterTowerWidget)
    explicit WaterTowerWidget(int id, QWidget *parent = 0);


private:
    Ui::WaterTowerWidget *ui;
    WaterTower *waterTower;

    QCheckBox *enableWidget;
    QSpinBox *barrelHeightWidget;
    QSpinBox *reservedHeightWidget;
    static QSpinBox *sampleIntervalWidget;

    static QMap<int, WaterTowerWidget*> instanceMap;
};

#endif // WATERTOWERWIDGET_H
