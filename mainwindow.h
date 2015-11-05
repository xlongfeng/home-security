#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QListWidgetItem;
class QGridLayout;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void pageChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void waterTowerLayoutChanged();

private:
    void createIcons();
    void insertWaterTowers(QGridLayout *layout);
    QWidget *createWaterTowers();
    QWidget *createBabyCare();
    QWidget *createOptions();
    QWidget *createWaterTowerOptions();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
