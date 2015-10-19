#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QListWidgetItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void pageChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    void createIcons();
    QWidget *createWaterTowers();
    QWidget *createBabyCare();
    QWidget *createOptions();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
