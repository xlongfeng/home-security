#include <QUdpSocket>
#include <QSettings>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->horizontalSlider0->setRange(0, 4);
    ui->horizontalSlider1->setRange(0, 4);
    ui->horizontalSlider2->setRange(0, 8);
    ui->horizontalSlider3->setRange(0, 8);
    ui->horizontalSlider4->setRange(0, 8);
    ui->horizontalSlider5->setRange(0, 8);

    sense = new QGraphicsScene(this);
    ui->graphicsView->setScene(sense);

    map = QPixmap("home.png");
    // mapItem = sense->addPixmap(QPixmap(1, 1));
    mapItem = sense->addPixmap(map);
    sense->addText("hello world");

    loadSettings();

    udp = new QUdpSocket(this);
    udp->bind(QHostAddress::LocalHost, 19999);

    connect(udp, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

void MainWindow::readPendingDatagrams()
{
    while (udp->hasPendingDatagrams()) {
        QByteArray data;
        data.resize(udp->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        udp->readDatagram(data.data(), data.size(), &sender, &senderPort);

        qDebug() << data.toHex() << sender << senderPort;

        quint8 id = data[0];
        if (id >= 0x10 && id < 0x20) {
            QByteArray resp;
            resp.append(id);
            resp.append('\0');
            int index = id - 0x10;
            bool isConnected;
            int value;
            switch (index) {
            case 0:
                isConnected = ui->checkBox0->isChecked();
                value = ui->horizontalSlider0->value();
                break;
            case 1:
                isConnected = ui->checkBox1->isChecked();
                value = ui->horizontalSlider1->value();
                break;
            case 2:
                isConnected = ui->checkBox2->isChecked();
                value = ui->horizontalSlider2->value();
                break;
            case 3:
                isConnected = ui->checkBox3->isChecked();
                value = ui->horizontalSlider3->value();
                break;
            case 4:
                isConnected = ui->checkBox4->isChecked();
                value = ui->horizontalSlider4->value();
                break;
            case 5:
                isConnected = ui->checkBox5->isChecked();
                value = ui->horizontalSlider5->value();
                break;
            default:
                value = 0;
                break;
            }
            value = (value + 1) * 5000;
            resp.append((value >> 0) & 0xFF);
            resp.append((value >> 8) & 0xFF);
            resp.append((value >> 16) & 0xFF);
            resp.append((value >> 24) & 0xFF);
            if (isConnected) {
                udp->writeDatagram(resp.data(), resp.size(), sender, senderPort);
            }
        }
    }
}

void MainWindow::loadSettings()
{
    QSettings settings("config.ini", QSettings::IniFormat);

    ui->checkBox0->setChecked(settings.value("WaterTower0/connect", false).toBool());
    ui->horizontalSlider0->setValue(settings.value("WaterTower0/level", 0).toInt());
    ui->checkBox1->setChecked(settings.value("WaterTower1/connect", false).toBool());
    ui->horizontalSlider1->setValue(settings.value("WaterTower1/level", 0).toInt());
    ui->checkBox2->setChecked(settings.value("WaterTower2/connect", false).toBool());
    ui->horizontalSlider2->setValue(settings.value("WaterTower2/level", 0).toInt());
    ui->checkBox3->setChecked(settings.value("WaterTower3/connect", false).toBool());
    ui->horizontalSlider3->setValue(settings.value("WaterTower3/level", 0).toInt());
    ui->checkBox4->setChecked(settings.value("WaterTower4/connect", false).toBool());
    ui->horizontalSlider4->setValue(settings.value("WaterTower4/level", 0).toInt());
    ui->checkBox5->setChecked(settings.value("WaterTower5/connect", false).toBool());
    ui->horizontalSlider5->setValue(settings.value("WaterTower5/level", 0).toInt());
}

void MainWindow::saveSettings()
{
    QSettings settings("config.ini", QSettings::IniFormat);

    settings.setValue("WaterTower0/connect", ui->checkBox0->isChecked());
    settings.setValue("WaterTower0/level", ui->horizontalSlider0->value());
    settings.setValue("WaterTower1/connect", ui->checkBox1->isChecked());
    settings.setValue("WaterTower1/level", ui->horizontalSlider1->value());
    settings.setValue("WaterTower2/connect", ui->checkBox2->isChecked());
    settings.setValue("WaterTower2/level", ui->horizontalSlider2->value());
    settings.setValue("WaterTower3/connect", ui->checkBox3->isChecked());
    settings.setValue("WaterTower3/level", ui->horizontalSlider3->value());
    settings.setValue("WaterTower4/connect", ui->checkBox4->isChecked());
    settings.setValue("WaterTower4/level", ui->horizontalSlider4->value());
    settings.setValue("WaterTower5/connect", ui->checkBox5->isChecked());
    settings.setValue("WaterTower5/level", ui->horizontalSlider5->value());
}

