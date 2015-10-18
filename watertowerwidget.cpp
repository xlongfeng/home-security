#include <QMessageBox>
#include <QDebug>

#include "watertower.h"
#include "watertowerwidget.h"
#include "ui_watertowerwidget.h"

WaterTowerWidget::WaterTowerWidget(int id, QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::WaterTowerWidget)
{
    ui->setupUi(this);

    setTitle(QString::number(id));
    waterTower = WaterTower::instance(id);
    connect(waterTower, SIGNAL(waterLevelChanged(quint32)), this, SLOT(waterLevelChanged(quint32)));

    ui->heightLineEdit->setText(QString::number(waterTower->getHeight()));
    ui->levelLineEdit->setText(QString::number(waterTower->getWaterLevel()));
    ui->avatarWidget->setPixmap(QPixmap(QString("%1.png").arg(id)));
}

WaterTowerWidget::~WaterTowerWidget()
{
    delete ui;
}

void WaterTowerWidget::waterLevelChanged(quint32 centimetre)
{
    ui->levelLineEdit->setText(QString::number(centimetre));
}

void WaterTowerWidget::highWaterLevelAlarm()
{
    QMessageBox messagebox(QMessageBox::NoIcon, title(), tr("High water level alarm!"), QMessageBox::Ok);
    messagebox.setIconPixmap(QPixmap(QString("%1.png").arg(waterTower->getIdentity())).scaledToHeight(256));
    messagebox.exec();
    waterTower->stopAlarm();
}
