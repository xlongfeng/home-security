#include <QMessageBox>
#include <QDebug>

#include "watertower.h"
#include "watertowerwidget.h"
#include "notifypanel.h"
#include "ui_watertowerwidget.h"

QSpinBox *WaterTowerWidget::sampleIntervalWidget = 0;
QMap<int, WaterTowerWidget*> WaterTowerWidget::instanceMap;

const char *connectStyle =
    "QProgressBar {"
        "border: 2px solid grey;"
        "border-radius: 5px;"
        "text-align: center;"
    "}"
    "QProgressBar::chunk {"
        "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                           "stop: 0 #%1, stop: 1.0 #0000FF);"
    "}";

const char *disconnectStyle =
    "QProgressBar {"
        "border: 2px solid grey;"
        "border-radius: 5px;"
        "text-align: center;"
    "}"
    "QProgressBar::chunk {"
        "background-color: lightGray;"
    "}";

WaterTowerWidget::WaterTowerWidget(int id, QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::WaterTowerWidget),
    uuid(NotifyPanel::instance()->uuid())
{
    ui->setupUi(this);

    setTitle(readableName(id));
    waterTower = WaterTower::instance(id);
    connect(waterTower, SIGNAL(waterLevelChanged(int)), this, SLOT(waterLevelChanged(int)));
    connect(waterTower, SIGNAL(deviceConnected()), this, SLOT(deviceConnect()));
    connect(waterTower, SIGNAL(deviceDisconnected()), this, SLOT(deviceDisconnect()));
    connect(waterTower, SIGNAL(highWaterLevelAlarm()), this, SLOT(highWaterLevelAlarm()));

    waterTower->getWaterLevel();
    ui->avatarWidget->setPixmap(QPixmap(QString(qApp->applicationDirPath() + "/images/watertower-%1.png").arg(id)));
    ui->progressBar->setRange(waterTower->waterLevelMinimum(), waterTower->waterLevelMaxminum());
    ui->progressBar->setFormat("%v");
    deviceDisconnect();
    connect(waterTower, SIGNAL(waterLevelRangeChanged(int,int)), ui->progressBar, SLOT(setRange(int,int)));

    enableWidget = new QCheckBox();
    enableWidget->setChecked(waterTower->isEnabled());

    enableAlarmWidget = new QCheckBox();
    enableAlarmWidget->setChecked(waterTower->isAlarmEnabled());

    addressWidget = new QSpinBox();
    addressWidget->setRange(0, 15);
    addressWidget->setValue(waterTower->getAddress());
    connect(addressWidget, SIGNAL(valueChanged(int)), this, SLOT(addressChanged(int)));

    radiusWidget = new QSpinBox();
    radiusWidget->setRange(20, 300);
    radiusWidget->setValue(waterTower->getRadius());
    connect(radiusWidget, SIGNAL(valueChanged(int)), this, SLOT(radiusChanged(int)));

    barrelHeightWidget = new QSpinBox();
    barrelHeightWidget->setRange(100, 500);
    barrelHeightWidget->setValue(waterTower->getHeight());
    connect(barrelHeightWidget, SIGNAL(valueChanged(int)), this, SLOT(barrelHeightChanged(int)));

    reservedHeightWidget = new QSpinBox();
    reservedHeightWidget->setRange(5, 500);
    reservedHeightWidget->setValue(waterTower->getHeightReserved());
    connect(reservedHeightWidget, SIGNAL(valueChanged(int)), this, SLOT(reservedHeightChanged(int)));

    getSampleIntervalWidget();
    connect(sampleIntervalWidget, SIGNAL(valueChanged(int)), this, SLOT(sampleIntervalChanged(int)));
    connect(enableWidget, SIGNAL(clicked(bool)), this, SLOT(readyForUse(bool)));
    connect(enableAlarmWidget, SIGNAL(clicked(bool)), this, SLOT(enableAlarm(bool)));
}

QString WaterTowerWidget::readableName(int id)
{
    switch(id) {
    case 0:
        return tr("Ground floor");
    case 1:
        return tr("Second floor");
    case 2:
        return tr("Seaside");
    case 3:
        return tr("Well");
    case 4:
        return tr("Boat");
    case 5:
        return tr("Null");
    default:
        return QString::number(id);
    }
}

/* static */
QWidget *WaterTowerWidget::getSampleIntervalWidget()
{
    if (!sampleIntervalWidget) {
        sampleIntervalWidget = new QSpinBox();
        sampleIntervalWidget->setRange(1, 30);
        sampleIntervalWidget->setValue(WaterTower::getSampleInterval());
    }
    return sampleIntervalWidget;
}

/* static */
WaterTowerWidget *WaterTowerWidget::instance(int identity)
{
    WaterTowerWidget *wt;

    if (instanceMap.contains(identity)) {
        wt = instanceMap.value(identity);
    } else {
        wt = new WaterTowerWidget(identity);
        instanceMap[identity] = wt;
    }
    return wt;
}

void WaterTowerWidget::sampleIntervalChanged(int value)
{
    WaterTower::setSampleInterval(value);
}

void WaterTowerWidget::readyForUse(bool checked)
{
    waterTower->setEnable(checked);
    emit layoutChanged();
}

void WaterTowerWidget::enableAlarm(bool checked)
{
    waterTower->setAlarmEnable(checked);
}

void WaterTowerWidget::addressChanged(int value)
{
    waterTower->setAddress(value);
}

void WaterTowerWidget::radiusChanged(int value)
{
    waterTower->setRadius(value);
}

void WaterTowerWidget::barrelHeightChanged(int value)
{
    waterTower->setHeight(value);
}

void WaterTowerWidget::reservedHeightChanged(int value)
{
    waterTower->setHeightReserved(value);
}

void WaterTowerWidget::waterLevelChanged(int centimetre)
{
    int maximum = ui->progressBar->maximum();
    int color = ((0xff * centimetre / maximum) << 16) + (0xff * (maximum - centimetre) / maximum);
    ui->progressBar->setStyleSheet(QString(connectStyle).arg(color, 6, 16, QLatin1Char('0')));
    ui->progressBar->setValue(centimetre);
    // ui->progressBar->setTextVisible(true);

    double radius = waterTower->getRadius() / 100.0;
    double volume = 3.1415926 * radius * radius * (centimetre / 100.0);
    ui->volumeLabel->setText(QString::number(volume, 'f', 1));
    ui->volumeLabel->setVisible(true);
    ui->unitLabel->setVisible(true);
}

void WaterTowerWidget::deviceConnect()
{

}

void WaterTowerWidget::deviceDisconnect()
{
    ui->progressBar->setStyleSheet(disconnectStyle);
    ui->progressBar->setValue((waterTower->waterLevelMinimum() + waterTower->waterLevelMaxminum()) / 2);
    ui->progressBar->setTextVisible(false);
    ui->volumeLabel->setVisible(false);
    ui->unitLabel->setVisible(false);
}

void WaterTowerWidget::highWaterLevelAlarm()
{
    NotifyPanel::instance()->addNotify(uuid, NotifyPanel::Middle,
            tr("%1: High water level!").arg(readableName(waterTower->getIdentity())),
            QString(qApp->applicationDirPath() + "/images/watertower-%1.png").arg(waterTower->getIdentity()));
    waterTower->stopAlarm();
}
