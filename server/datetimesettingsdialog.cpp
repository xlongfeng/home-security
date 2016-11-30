#include <QProcess>
#include <QDebug>
#include "settings.h"
#include "datetimesettingsdialog.h"
#include "ui_datetimesettingsdialog.h"

DateTimeSettingsDialog::DateTimeSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DateTimeSettingsDialog),
    dateTime(QDateTime::currentDateTime())
{
    ui->setupUi(this);
    setWindowTitle(tr("Date and Time Settings"));

    bool timeApDisplayFormat = Settings::instance()->value("TimeApDisplayFormat", false).toBool();
    ui->twentyFourHourRadioButton->setChecked(!timeApDisplayFormat);
    ui->apRadioButton->setChecked(timeApDisplayFormat);

    dateTimeUpdated();

    connect(ui->yearPlusButton, SIGNAL(pressed()), this, SLOT(yearPlus()));
    connect(ui->yearMinusButton, SIGNAL(pressed()), this, SLOT(yearMinus()));

    connect(ui->monthPlusButton, SIGNAL(pressed()), this, SLOT(monthPlus()));
    connect(ui->monthMinusButton, SIGNAL(pressed()), this, SLOT(monthMinus()));

    connect(ui->dayPlusButton, SIGNAL(pressed()), this, SLOT(dayPlus()));
    connect(ui->dayMinusButton, SIGNAL(pressed()), this, SLOT(dayMinus()));

    connect(ui->hourPlusButton, SIGNAL(pressed()), this, SLOT(hourPlus()));
    connect(ui->hourMinusButton, SIGNAL(pressed()), this, SLOT(hourMinus()));

    connect(ui->minutePlusButton, SIGNAL(pressed()), this, SLOT(minutePlus()));
    connect(ui->minuteMinusButton, SIGNAL(pressed()), this, SLOT(minuteMinus()));

    connect(this, SIGNAL(accepted()), this, SLOT(setHwClock()));
}

DateTimeSettingsDialog::~DateTimeSettingsDialog()
{
    delete ui;
}

void DateTimeSettingsDialog::setHwClock()
{
    const QString &program = "date";
    QStringList options =
            QStringList() << "-s"
            << QString("%1 %2")
               .arg(dateTime.date().toString("yyyy-MM-dd"))
               .arg(dateTime.time().toString("hh:mm:ss"));
    qDebug() << program << options;

#ifdef __arm__
    QProcess::execute(program, options);
    QProcess::execute("hwclock", QStringList() << "-w");
#endif

    Settings::instance()->setValue("TimeApDisplayFormat", ui->apRadioButton->isChecked());
}

void DateTimeSettingsDialog::minutePlus()
{
    dateTime = dateTime.addSecs(60);
    dateTimeUpdated();
}

void DateTimeSettingsDialog::minuteMinus()
{
    dateTime = dateTime.addSecs(-60);
    dateTimeUpdated();
}

void DateTimeSettingsDialog::hourPlus()
{
    dateTime = dateTime.addSecs(3600);
    dateTimeUpdated();
}

void DateTimeSettingsDialog::hourMinus()
{
    dateTime = dateTime.addSecs(-3600);
    dateTimeUpdated();
}

void DateTimeSettingsDialog::dayPlus()
{
    dateTime = dateTime.addDays(1);
    dateTimeUpdated();
}

void DateTimeSettingsDialog::dayMinus()
{
    dateTime = dateTime.addDays(-1);
    dateTimeUpdated();
}

void DateTimeSettingsDialog::monthPlus()
{
    dateTime = dateTime.addMonths(1);
    dateTimeUpdated();
}

void DateTimeSettingsDialog::monthMinus()
{
    dateTime = dateTime.addMonths(-1);
    dateTimeUpdated();
}

void DateTimeSettingsDialog::yearPlus()
{
    dateTime = dateTime.addYears(1);
    dateTimeUpdated();
}

void DateTimeSettingsDialog::yearMinus()
{
    dateTime = dateTime.addYears(-1);
    dateTimeUpdated();
}

void DateTimeSettingsDialog::dateTimeUpdated()
{
    minuteUpdated();
    hourUpdated();
    dayUpdated();
    monthUpdated();
    yearUpdated();
}

void DateTimeSettingsDialog::minuteUpdated()
{
    ui->minuteLabel->setText(QString::number(dateTime.time().minute()));
}

void DateTimeSettingsDialog::hourUpdated()
{
    ui->hourLabel->setText(QString::number(dateTime.time().hour()));
}

void DateTimeSettingsDialog::dayUpdated()
{
    ui->dayLabel->setText(QString::number(dateTime.date().day()));
}

void DateTimeSettingsDialog::monthUpdated()
{
    ui->monthLabel->setText(QString::number(dateTime.date().month()));
}

void DateTimeSettingsDialog::yearUpdated()
{
    ui->yearLabel->setText(QString::number(dateTime.date().year()));
}
