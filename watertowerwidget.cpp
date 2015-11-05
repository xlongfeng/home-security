#include <QMessageBox>
#include <QDebug>

#include "watertower.h"
#include "watertowerwidget.h"
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

WaterTowerWidget::WaterTowerWidget(int id, QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::WaterTowerWidget)
{
    ui->setupUi(this);

    setTitle(ReadableName[id]);
    waterTower = WaterTower::instance(id);
    connect(waterTower, SIGNAL(waterLevelChanged(quint32, int)), this, SLOT(waterLevelChanged(quint32, int)));

    ui->heightLineEdit->setText(QString::number(waterTower->getHeight()));
    ui->levelLineEdit->setText(QString::number(waterTower->getWaterLevel()));
    ui->avatarWidget->setPixmap(QPixmap(QString("%1.png").arg(id)));
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(0);

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
    ui->levelLineEdit->setText(QString::number(centimetre));
    ui->progressBar->setValue(progress);
}

void WaterTowerWidget::highWaterLevelAlarm()
{
    QMessageBox messagebox(QMessageBox::NoIcon, title(), tr("High water level alarm!"), QMessageBox::Ok);
    messagebox.setIconPixmap(QPixmap(QString("%1.png").arg(waterTower->getIdentity())).scaledToHeight(256));
    messagebox.exec();
    waterTower->stopAlarm();
}
