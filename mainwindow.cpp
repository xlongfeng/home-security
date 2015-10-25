#include <QTabWidget>
#include "watertowerwidget.h"
#include "babycare.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->listWidget->setViewMode(QListView::IconMode);
    ui->listWidget->setIconSize(QSize(96, 96));
    ui->listWidget->setMovement(QListView::Static);
    ui->listWidget->setSpacing(12);

    createIcons();

    ui->stackedWidget->insertWidget(0, createWaterTowers());
    ui->stackedWidget->insertWidget(1, createBabyCare());
    ui->stackedWidget->insertWidget(2, createOptions());

    ui->listWidget->setCurrentRow(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::pageChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    ui->stackedWidget->setCurrentIndex(ui->listWidget->row(current));
}

void MainWindow::createIcons()
{
    QListWidgetItem *waterTowerButton = new QListWidgetItem(ui->listWidget);
    waterTowerButton->setIcon(QIcon("watertower.png"));
    waterTowerButton->setText(tr("Water Tower"));
    waterTowerButton->setTextAlignment(Qt::AlignHCenter);
    waterTowerButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *babyCareButton = new QListWidgetItem(ui->listWidget);
    babyCareButton->setIcon(QIcon("babycare.png"));
    babyCareButton->setText(tr("Baby Care"));
    babyCareButton->setTextAlignment(Qt::AlignHCenter);
    babyCareButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *optionsButton = new QListWidgetItem(ui->listWidget);
    optionsButton->setIcon(QIcon("options.png"));
    optionsButton->setText(tr("Options"));
    optionsButton->setTextAlignment(Qt::AlignHCenter);
    optionsButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    connect(ui->listWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(pageChanged(QListWidgetItem*,QListWidgetItem*)));
}

QWidget *MainWindow::createWaterTowers()
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

    return waterTowerLayout;
}

QWidget *MainWindow::createBabyCare()
{
    return new BabyCare(this);
}

QWidget *MainWindow::createOptions()
{
    QTabWidget *tabWidget = new QTabWidget(this);
    tabWidget->addTab(new QWidget, tr("Water Tower"));
    tabWidget->addTab(new QWidget, tr("Baby Care"));
    tabWidget->addTab(new QWidget, tr("General"));
    return tabWidget;
}
