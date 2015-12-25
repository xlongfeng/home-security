#ifndef NOTIFYPANEL_H
#define NOTIFYPANEL_H

#include <QDialog>
#include <QStringList>
#include <QMap>
#include <QMediaPlayer>
#include <QMediaPlaylist>

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
        High,
        None
    };
    QString uuid() const;
    void addNotify(const QString &uuid, Priority priority, const QString &text, const QString &icon = "");
    static NotifyPanel *instance();

private slots:
    void confirm();
    void blink();

private:
    explicit NotifyPanel(QWidget *parent = 0);
    Q_DISABLE_COPY(NotifyPanel)
    void nextNotify();
    void showNotify(Priority priority, const QStringList &notify);
    void ledsOff();

private:
    static NotifyPanel *self;
    QList<QStringList> lowPriorityNotifies;
    QList<QStringList> middlePriorityNotifies;
    QList<QStringList> highPriorityNotifies;

    QString currentUuid;
    Priority currentPriority;

    AvatarWidget *avatar;
    QLabel *message;

    QMap<Priority, int> blinkIntervalMap;

    bool isOn;
    QTimer *timer;

    QMap<Priority, QString> mediaMap;
    QMediaPlayer *player;
    QMediaPlaylist *playlist;
};

#endif // NOTIFYPANEL_H
