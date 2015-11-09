#include <QMessageBox>
#include <QDebug>

#include "watertower.h"
#include "watertowerwidget.h"
#include "notifypanel.h"
#include "ui_watertowerwidget.h"

QSpinBox *WaterTowerWidget::sampleIntervalWidget = 0;
QMap<int, WaterTowerWidget*> WaterTowerWidget::instanceMap;

static QStringList ReadableName = QStringList()
                            << QObject::tr("Ground floor")
                            << QObject::tr("Second floor")
                            << QObject::tr("Seaside")
                            << QObject::tr("Well")
                            << QObject::tr("Boat")
                            << QObject::tr("Null");

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
    ui(new Ui::WaterTowerWidget)
{
    ui->setupUi(this);

    setTitle(ReadableName[id]);
    waterTower = WaterTower::instance(id);
    connect(waterTower, SIGNAL(waterLevelChanged(quint32, int)), this, SLOT(waterLevelChanged(quint32, int)));
    connect(waterTower, SIGNAL(deviceConnected()), this, SLOT(deviceConnect()));
    connect(waterTower, SIGNAL(deviceDisconnected()), this, SLOT(deviceDisconnect()));
    connect(waterTower, SIGNAL(highWaterLevelAlarm()), this, SLOT(highWaterLevelAlarm()));

    ui->heightLineEdit->setText(QString::number(waterTower->getHeight()));
    ui->levelLineEdit->setText(QString::number(waterTower->getWaterLevel()));
    ui->avatarWidget->setPixmap(QPixmap(QString("%1.png").arg(id)));
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(50);
    ui->progressBar->setTextVisible(false);
    ui->progressBar->setStyleSheet(disconnectStyle);

    enableWidget = new QCheckBox(this);
    enableWidget->setChecked(waterTower->isEnabled());

    barrelHeightWidget = new QSpinBox(this);
    barrelHeightWidget->setRange(100, 500);
    barrelHeightWidget->setValue(waterTower->getHeight());
    connect(barrelHeightWidget, SIGNAL(valueChanged(int)), this, SLOT(barrelHeightChanged(int)));

    reservedHeightWidget = new QSpinBox(this);
    reservedHeightWidget->setRange(5, 500);
    reservedHeightWidget->setValue(waterTower->getHeightReserved());
    connect(reservedHeightWidget, SIGNAL(valueChanged(int)), this, SLOT(reservedHeightChanged(int)));

    getSampleIntervalWidget();
    connect(sampleIntervalWidget, SIGNAL(valueChanged(int)), this, SLOT(sampleIntervalChanged(int)));
    connect(enableWidget, SIGNAL(clicked(bool)), this, SLOT(readyForUse(bool)));
}

QString WaterTowerWidget::readableName(int id)
{
    if (id < WaterTower::MaxQuantity) {
        return ReadableName[id];
    } else
        return QString::number(id);
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

void WaterTowerWidget::barrelHeightChanged(int value)
{
    waterTower->setHeight(value);
    ui->heightLineEdit->setText(QString::number(value));
}

void WaterTowerWidget::reservedHeightChanged(int value)
{
    waterTower->setHeightReserved(value);
}

void WaterTowerWidget::waterLevelChanged(quint32 centimetre, int progress)
{
    int color = ((0xff * progress / 100) << 16) + (0xff * (100 - progress) / 100);
    ui->progressBar->setStyleSheet(QString(connectStyle).arg(color, 6, 16, QLatin1Char('0')));
    ui->levelLineEdit->setText(QString::number(centimetre));
    ui->progressBar->setValue(progress);
}

void WaterTowerWidget::deviceConnect()
{

}

void WaterTowerWidget::deviceDisconnect()
{
    ui->progressBar->setStyleSheet(disconnectStyle);
}

void WaterTowerWidget::highWaterLevelAlarm()
{
    NotifyPanel::instance()->addNotify(NotifyPanel::Middle,
            tr("%1: High water level alarm!").arg(readableName(waterTower->getIdentity())),
            QString("%1.png").arg(waterTower->getIdentity()));
    waterTower->stopAlarm();
}
