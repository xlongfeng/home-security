#ifndef NOTIFYPANEL_H
#define NOTIFYPANEL_H

#include <QDialog>
#include <QStringList>
#include <QMap>
#include <QMediaPlayer>

class QLabel;
class QTimer;
class AvatarWidget;

class NotifyPanel : public QDialog
{
    Q_OBJECT

public:
    enum Priority {
        Low,
        Middle,
        High
    };
    void addNotify(Priority priority, const QString &text, const QString &icon = "");
    static NotifyPanel *instance();

private slots:
    void confirm();
    void blink();
    void playerStateChanged(QMediaPlayer::State state);

private:
    explicit NotifyPanel(QWidget *parent = 0);
    Q_DISABLE_COPY(NotifyPanel)
    void nextNotify();
    void showNotify(Priority priority, const QStringList &notify);
    void setLedValue(const QString &led, bool on);
    void ledsOff();

private:
    static NotifyPanel *self;
    QList<QStringList> lowPriorityNotifies;
    QList<QStringList> middlePriorityNotifies;
    QList<QStringList> highPriorityNotifies;

    QString uuid;
    Priority priority;

    AvatarWidget *avatar;
    QLabel *text;

    QMap<Priority, int> blinkIntervalMap;

    bool isOn;
    QMap<Priority, QString> ledMap;
    QTimer *timer;

    QMap<Priority, QString> mediaMap;
    QMediaPlayer *player;
};

#endif // NOTIFYPANEL_H
