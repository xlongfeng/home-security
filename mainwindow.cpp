#include <QLabel>
#include <QTabWidget>
#include <QTableWidget>
#include <QDebug>

#include "watertower.h"
#include "watertowerwidget.h"
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
    ui->listWidget->setFixedWidth(128);

    createIcons();

    ui->stackedWidget->insertWidget(0, createWaterTowers());
    ui->stackedWidget->insertWidget(1, createBabyCare());
    ui->stackedWidget->insertWidget(2, createOptions());

    ui->listWidget->setCurrentRow(0);

    for (int i = 0; i < WaterTower::MaxQuantity; i++) {
        connect(WaterTowerWidget::instance(i), SIGNAL(layoutChanged()), this, SLOT(waterTowerLayoutChanged()));
    }
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

void MainWindow::waterTowerLayoutChanged()
{
    QWidget *widget = ui->stackedWidget->widget(0);
    QGridLayout *layout = qobject_cast<QGridLayout *>(widget->layout());
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != 0) {
        item->widget()->setParent(0);
        delete item;
    }
    insertWaterTowers(layout);
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

void MainWindow::insertWaterTowers(QGridLayout *layout)
{
    static const struct {
        int row;
        int col;
    } position[] = {
        {0, 0},
        {0, 1},
        {0, 2},
        {1, 0},
        {1, 1},
        {1, 2},
    };

    int pos = 0;
    for (int i = 0; i < WaterTower::MaxQuantity; i++) {
        if (WaterTower::instance(i)->isEnabled()) {
            layout->addWidget(WaterTowerWidget::instance(i), position[pos].row, position[pos].col);
            pos++;
        }
    }
}

QWidget *MainWindow::createWaterTowers()
{
    QWidget *waterTowers= new QWidget(this);
    QGridLayout *layout = new QGridLayout();
    waterTowers->setLayout(layout);

    insertWaterTowers(layout);

    return waterTowers;
}

QWidget *MainWindow::createBabyCare()
{
    return new QWidget(this);
}

QWidget *MainWindow::createOptions()
{
    QTabWidget *tabWidget = new QTabWidget(this);
    tabWidget->addTab(createWaterTowerOptions(), tr("Water Tower"));
    tabWidget->addTab(new QWidget, tr("Baby Care"));
    tabWidget->addTab(new QWidget, tr("General"));
    return tabWidget;
}

QWidget *MainWindow::createWaterTowerOptions()
{
    QWidget *option = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout();
    option->setLayout(layout);

    QWidget *globalOption = new QWidget(option);
    {
        QHBoxLayout *layout = new QHBoxLayout();
        globalOption->setLayout(layout);
        layout->addWidget(new QLabel(tr("Sample Interval"), globalOption));
        layout->addWidget(WaterTowerWidget::getSampleIntervalWidget());
        QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        layout->addSpacerItem(spacer);
    }

    layout->addWidget(globalOption);

#if 0
    QFrame *line = new QFrame(option);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addWidget(line);
#endif

    QTableWidget * table = new QTableWidget(0, 4, option);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setAlternatingRowColors(true);
    table->setShowGrid(true);
    table->setHorizontalHeaderLabels(QStringList()
        << tr("Identity") << tr("Enable")
        << tr("Barrel Height") << tr("Reserved Height"));
    for (int i = 0; i < WaterTower::MaxQuantity; i++) {
        table->insertRow(i);
        QTableWidgetItem *id = new QTableWidgetItem;
        id->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        id->setText(WaterTowerWidget::instance(i)->readableName(i));
        table->setItem(i, 0, id);
        table->setCellWidget(i, 1, WaterTowerWidget::instance(i)->getEnableWidget());
        table->setCellWidget(i, 2, WaterTowerWidget::instance(i)->getBarrelHeightWidget());
        table->setCellWidget(i, 3, WaterTowerWidget::instance(i)->getReservedHeightWidget());
    }
    layout->addWidget(table);

    return option;
}
