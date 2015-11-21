#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include <QMainWindow>
#include <QMediaPlayer>

namespace Ui {
class MainWindow;
}

class QDateTimeEdit;
class QListWidgetItem;
class QGridLayout;
class QSlider;
class QTimer;

class QuickDialog : public QDialog
{
    Q_OBJECT

public:
    QuickDialog(const QString &label, int value, int minimum, int maximum, QWidget *parent = 0);

signals:
    void valueChanged(int value);

public slots:
    void setValue(int value);


protected:
    void keyPressEvent(QKeyEvent *event);

private:
    QTimer *timer;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void pageChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void waterTowerLayoutChanged();
    void dateTimeUpdate();
    void brightnessChanged(int value);
    void volumeChanged(int value);
    void dateTimeSettings();
    void playerStateChanged(QMediaPlayer::State state);

protected:
    virtual void keyPressEvent(QKeyEvent *event);

private:
    void dateTimeDisplayFormat();
    void createIcons();
    void insertWaterTowers(QGridLayout *layout);
    QWidget *createWaterTowers();
    QWidget *createBabyCare();
    QWidget *createOptions();
    QWidget *createWaterTowerOptions();
    QWidget *createGeneralOptions();

private:
    Ui::MainWindow *ui;
    QSlider *brightnessSilder;
    QSlider *volumeSilder;
    QDateTimeEdit *dateTime;
    QMediaPlayer *player;
    bool oneMoreCycle;
};

#endif // MAINWINDOW_H
