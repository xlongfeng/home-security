#include "watertowerwidget.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    addWaterTowers();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addWaterTowers()
{
    QWidget *waterTowerLayout = new QWidget(this);
    QGridLayout *gridLayout = new QGridLayout();
    waterTowerLayout->setLayout(gridLayout);

    gridLayout->addWidget(new WaterTowerWidget(0, this), 0, 0);
    gridLayout->addWidget(new WaterTowerWidget(1, this), 0, 1);
    gridLayout->addWidget(new WaterTowerWidget(2, this), 0, 2);
    gridLayout->addWidget(new WaterTowerWidget(3, this), 1, 0);
    gridLayout->addWidget(new WaterTowerWidget(4, this), 1, 1);
    gridLayout->addWidget(new WaterTowerWidget(5, this), 1, 2);

    ui->stackedWidget->insertWidget(0, waterTowerLayout);
}
