#ifndef WATERTOWERWIDGET_H
#define WATERTOWERWIDGET_H

#include <QGroupBox>

namespace Ui {
class WaterTowerWidget;
}

class WaterTower;

class WaterTowerWidget : public QGroupBox
{
    Q_OBJECT

public:
    explicit WaterTowerWidget(int id, QWidget *parent = 0);
    ~WaterTowerWidget();

public slots:
    void waterLevelChanged(quint32 centimetre);
    void highWaterLevelAlarm();

private:
    Ui::WaterTowerWidget *ui;
    WaterTower *waterTower;
};

#endif // WATERTOWERWIDGET_H
