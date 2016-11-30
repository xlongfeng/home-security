#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QGraphicsScene;
class QGraphicsPixmapItem;
class QUdpSocket;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void readPendingDatagrams();

private:
    void loadSettings();
    void saveSettings();

private:
    Ui::MainWindow *ui;

    QGraphicsScene *sense;
    QGraphicsPixmapItem *mapItem;
    QPixmap map;

    QUdpSocket *udp;
};

#endif // MAINWINDOW_H
