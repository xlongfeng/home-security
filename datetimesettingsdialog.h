#ifndef DATETIMESETTINGSDIALOG_H
#define DATETIMESETTINGSDIALOG_H

#include <QDialog>
#include <QDateTime>

namespace Ui {
class DateTimeSettingsDialog;
}

class DateTimeSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DateTimeSettingsDialog(QWidget *parent = 0);
    ~DateTimeSettingsDialog();

private slots:
    void setHwClock();
    void minutePlus();
    void minuteMinus();
    void hourPlus();
    void hourMinus();
    void dayPlus();
    void dayMinus();
    void monthPlus();
    void monthMinus();
    void yearPlus();
    void yearMinus();

private:
    void dateTimeUpdated();
    void minuteUpdated();
    void hourUpdated();
    void dayUpdated();
    void monthUpdated();
    void yearUpdated();

private:
    Ui::DateTimeSettingsDialog *ui;

    QDateTime dateTime;
};

#endif // DATETIMESETTINGSDIALOG_H
