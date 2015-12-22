#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>
#include <QDateTimeEdit>
#include <QTimer>
#include <QFormLayout>
#include <QSlider>
#include <QDialog>
#include <QDir>
#include <QKeyEvent>
#include <QDebug>

#include "watertower.h"
#include "watertowerwidget.h"
#include "babycare.h"
#include "datetimesettingsdialog.h"
#include "mainwindow.h"
#include "settings.h"
#include "hal.h"
#include "ui_mainwindow.h"

QuickDialog::QuickDialog(const QString &label, int value, int minimum, int maximum, QWidget *parent) :
    QDialog(parent)
{
    QFormLayout *layout = new QFormLayout(this);
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setMinimumWidth(240);
    slider->setRange(minimum, maximum);
    slider->setValue(value);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
    layout->addRow(label, slider);

    timer = new QTimer(this);
    timer->start(5000);
    connect(timer, SIGNAL(timeout()), this, SLOT(accept()));
}

void QuickDialog::setValue(int value)
{
    timer->start();
    emit valueChanged(value);
}

void QuickDialog::keyPressEvent(QKeyEvent *event)
{
    accept();
    QDialog::keyPressEvent(event);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    oneMoreCycle(false)
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

    dateTime = new QDateTimeEdit(QDateTime::currentDateTime(), this);
    dateTime->setReadOnly(true);
    dateTime->setButtonSymbols(QAbstractSpinBox::NoButtons);
    dateTimeDisplayFormat();
    ui->statusBar->addPermanentWidget(dateTime);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(dateTimeUpdate()));
    timer->start(1000);

    Hal::instance()->setBrightness(Settings::instance()->getBrightness());

    player = new QMediaPlayer(this);
    player->setMedia(QUrl::fromLocalFile(QString("%1/audios/%2.mp3").arg(qApp->applicationDirPath()).arg(QLatin1String("volume"))));
    connect(player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(playerStateChanged(QMediaPlayer::State)));
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

void MainWindow::dateTimeUpdate()
{
    dateTime->setDateTime(QDateTime::currentDateTime());
}

void MainWindow::brightnessChanged(int value)
{
    Settings::instance()->setBrightness(value);
    Hal::instance()->setBrightness(value);
}

void MainWindow::volumeChanged(int value)
{
    value *= 10;
    Settings::instance()->setVolume(value);
    player->setVolume(value);
    if (player->state() == QMediaPlayer::StoppedState) {
        oneMoreCycle = false;
        player->play();
    } else if (player->state() == QMediaPlayer::PlayingState) {
        qint64 duration = player->duration();
        qint64 position = player->position();
        if (position * 3 < duration) {
            oneMoreCycle = false;
        } else {
            oneMoreCycle = true;
        }
    } else {

    }
}

void MainWindow::dateTimeSettings()
{
    DateTimeSettingsDialog dialog;
    if(dialog.exec())
        dateTimeDisplayFormat();
}

void MainWindow::playerStateChanged(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::StoppedState && oneMoreCycle == true) {
        oneMoreCycle = false;
        player->play();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_F1:
    {
        QuickDialog *dialog = new QuickDialog(tr("Volume"), Settings::instance()->getVolume() / 10, 1, 10);
        connect(dialog, SIGNAL(valueChanged(int)), volumeSilder, SLOT(setValue(int)));
        dialog->exec();
        delete dialog;
        event->accept();
        break;
    }
    case Qt::Key_F2:
    {
        int maxBrightness = Hal::instance()->getMaxBrightness();
        int brightness = Settings::instance()->getBrightness();
        brightness = brightness < maxBrightness ? brightness : maxBrightness;
        QuickDialog *dialog = new QuickDialog(tr("Brightness"), brightness, 1, maxBrightness);
        connect(dialog, SIGNAL(valueChanged(int)), brightnessSilder, SLOT(setValue(int)));
        dialog->exec();
        delete dialog;
        event->accept();
        break;
    }
    case Qt::Key_PowerOff:
    {
        Hal::instance()->togglePower();
        break;
    }
    default:
        QMainWindow::keyPressEvent(event);
        break;
    }
}

void MainWindow::dateTimeDisplayFormat()
{
    bool timeApDisplayFormat = Settings::instance()->value("TimeApDisplayFormat", false).toBool();
    if (timeApDisplayFormat)
        dateTime->setDisplayFormat("yyyy/MM/dd HH:mm:ss AP");
    else
        dateTime->setDisplayFormat("yyyy/MM/dd HH:mm:ss");
}

void MainWindow::createIcons()
{
    QListWidgetItem *waterTowerButton = new QListWidgetItem(ui->listWidget);
    waterTowerButton->setIcon(QIcon(qApp->applicationDirPath() + "/images/watertower.png"));
    waterTowerButton->setText(tr("Water Tower"));
    waterTowerButton->setTextAlignment(Qt::AlignHCenter);
    waterTowerButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *babyCareButton = new QListWidgetItem(ui->listWidget);
    babyCareButton->setIcon(QIcon(qApp->applicationDirPath() + "/images/babycare.png"));
    babyCareButton->setText(tr("Baby Care"));
    babyCareButton->setTextAlignment(Qt::AlignHCenter);
    babyCareButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *optionsButton = new QListWidgetItem(ui->listWidget);
    optionsButton->setIcon(QIcon(qApp->applicationDirPath() + "/images/options.png"));
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
    return new BabyCare(this);
}

QWidget *MainWindow::createOptions()
{
    QTabWidget *tabWidget = new QTabWidget(this);
    tabWidget->addTab(createWaterTowerOptions(), tr("Water Tower"));
    tabWidget->addTab(new QWidget, tr("Baby Care"));
    tabWidget->addTab(createGeneralOptions(), tr("General"));
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

    QTableWidget * table = new QTableWidget(0, 6, option);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setAlternatingRowColors(true);
    table->setShowGrid(true);
    table->setHorizontalHeaderLabels(QStringList()
        << tr("Name") << tr("Enable") << tr("Alarm") << tr("Address")
        << tr("Barrel Height") << tr("Reserved Height"));
    for (int i = 0; i < WaterTower::MaxQuantity; i++) {
        table->insertRow(i);
        QTableWidgetItem *id = new QTableWidgetItem;
        id->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        id->setText(WaterTowerWidget::instance(i)->readableName(i));
        table->setItem(i, 0, id);
        table->setCellWidget(i, 1, WaterTowerWidget::instance(i)->getEnableWidget());
        table->setCellWidget(i, 2, WaterTowerWidget::instance(i)->getAlarmEnableWidget());
        table->setCellWidget(i, 3, WaterTowerWidget::instance(i)->getAddressWidget());
        table->setCellWidget(i, 4, WaterTowerWidget::instance(i)->getBarrelHeightWidget());
        table->setCellWidget(i, 5, WaterTowerWidget::instance(i)->getReservedHeightWidget());
    }
    layout->addWidget(table);

    return option;
}

QWidget *MainWindow::createGeneralOptions()
{
    QWidget *option = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(option);

    QFormLayout *leftLayout = new QFormLayout();
    layout->addLayout(leftLayout, 1);

    brightnessSilder = new QSlider(Qt::Horizontal);
    int maxBrightness = Hal::instance()->getMaxBrightness();
    int brightness = Settings::instance()->getBrightness();
    brightness = brightness < maxBrightness ? brightness : maxBrightness;
    brightnessSilder->setRange(1, maxBrightness);
    brightnessSilder->setValue(brightness);
    connect(brightnessSilder, SIGNAL(valueChanged(int)), this, SLOT(brightnessChanged(int)));
    leftLayout->addRow(new QLabel(tr("Brightness")), brightnessSilder);

    volumeSilder = new QSlider(Qt::Horizontal);
    volumeSilder->setRange(1, 10);
    volumeSilder->setValue(Settings::instance()->getVolume() / 10);
    connect(volumeSilder, SIGNAL(valueChanged(int)), this, SLOT(volumeChanged(int)));
    leftLayout->addRow(new QLabel(tr("Volume")), volumeSilder);

    QFrame *line = new QFrame(option);
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addWidget(line);

    QFormLayout *rightLayout = new QFormLayout();
    layout->addLayout(rightLayout, 1);

    QPushButton *dateTimeSettingsButton = new QPushButton(tr("Date and Time Settings"));
    connect(dateTimeSettingsButton, SIGNAL(pressed()), SLOT(dateTimeSettings()));
    rightLayout->addWidget(dateTimeSettingsButton);

    return option;
}
