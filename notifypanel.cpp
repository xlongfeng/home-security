#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QDialogButtonBox>
#include <QLabel>
#include <QUuid>
#include <QMediaPlayer>
#include <QDir>
#include <QTimer>
#include <QDebug>

#include "avatarwidget.h"
#include "notifypanel.h"
#include "settings.h"
#include "hal.h"


NotifyPanel *NotifyPanel::self = 0;

NotifyPanel::NotifyPanel(QWidget *parent) :
    QDialog(parent)
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);

    QHBoxLayout *hLayout = new QHBoxLayout();
    vLayout->addLayout(hLayout);

    avatar = new AvatarWidget(this);
    avatar->setFixedSize(128, 128);
    hLayout->addWidget(avatar);
    message = new QLabel(this);
    hLayout->addWidget(message);

    QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    hLayout->addSpacerItem(spacer);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    connect(buttons, SIGNAL(accepted()), this, SLOT(confirm()));
    vLayout->addWidget(buttons);

    blinkIntervalMap[Low] = 3000;
    blinkIntervalMap[Middle] = 2000;
    blinkIntervalMap[High] = 1000;

    isOn = false;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(blink()));

    ledsOff();

    mediaMap[Low] = QString("%1/audios/%2.mp3").arg(qApp->applicationDirPath()).arg(QLatin1String("low"));
    mediaMap[Middle] = QString("%1/audios/%2.mp3").arg(qApp->applicationDirPath()).arg(QLatin1String("middle"));
    mediaMap[High] = QString("%1/audios/%2.mp3").arg(qApp->applicationDirPath()).arg(QLatin1String("high"));

    player = new QMediaPlayer(this);
    player->setVolume(Settings::instance()->getVolume());
    connect(Settings::instance(), SIGNAL(volumeChanged(int)), player, SLOT(setVolume(int)));
    connect(player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(playerStateChanged(QMediaPlayer::State)));
}

QString NotifyPanel::uuid() const
{
    return QUuid::createUuid().toString();
}

void NotifyPanel::addNotify(const QString &uuid, Priority priority, const QString &text, const QString &icon)
{
    if (uuid == currentUuid) {
        message->setText(text);
        avatar->setPixmap(QPixmap(icon));
        return;
    }

    QList<QStringList> *notifies;
    switch (priority) {
    case Low:
        notifies = &lowPriorityNotifies;
        break;
    case Middle:
        notifies = &middlePriorityNotifies;
        break;
    case High:
        notifies = &highPriorityNotifies;
        break;
    default:
        notifies = 0;
        break;
    }
    if (notifies && notifies->count() < 8) {
        QListIterator<QStringList> i(*notifies);
        while (i.hasNext()) {
            const QStringList &notify = i.next();
            notifies->removeOne(notify);
        }
        notifies->append(QStringList() << uuid << text << icon);
        nextNotify();
        Hal::instance()->powerOn();
    }
}

NotifyPanel *NotifyPanel::instance()
{
    if (!self)
        self = new NotifyPanel();
    return self;
}

void NotifyPanel::confirm()
{
    currentUuid = "";
    nextNotify();
    if (currentUuid.isEmpty()) {
        ledsOff();
        player->stop();
        accept();
    }
}

void NotifyPanel::blink()
{
    isOn = !isOn;

    switch (priority) {
    case Low:
        Hal::instance()->setBlueLed(isOn);
        break;
    case Middle:
        Hal::instance()->setYellowLed(isOn);
        break;
    case High:
        Hal::instance()->setRedLed(isOn);
        break;
    default:
        break;
    }
}

void NotifyPanel::playerStateChanged(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::StoppedState && !currentUuid.isEmpty()) {
        player->play();
    }
}

void NotifyPanel::nextNotify()
{
    if(currentUuid.isEmpty()) {
        if (!highPriorityNotifies.isEmpty()) {
            showNotify(High, highPriorityNotifies.takeFirst());
        } else if (!middlePriorityNotifies.isEmpty()) {
            showNotify(Middle, middlePriorityNotifies.takeFirst());
        } else if (!lowPriorityNotifies.isEmpty()) {
            showNotify(Low, lowPriorityNotifies.takeFirst());
        } else {

        }
    }
}

void NotifyPanel::showNotify(Priority priority, const QStringList &notify)
{
    if (this->priority != priority || player->state() == QMediaPlayer::StoppedState) {
        this->priority = priority;
        ledsOff();
        timer->start(blinkIntervalMap[priority]);
        player->stop();
        player->setMedia(QUrl::fromLocalFile(mediaMap[priority]));
        player->play();
    }

    currentUuid = notify[0];
    message->setText(notify[1]);
    avatar->setPixmap(QPixmap(notify[2]));
    if (!isVisible()) {
        exec();
    }
}

void NotifyPanel::ledsOff()
{
    timer->stop();
    isOn = false;
    Hal::instance()->setBlueLed(isOn);
    Hal::instance()->setYellowLed(isOn);
    Hal::instance()->setRedLed(isOn);
}
