#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QDialogButtonBox>
#include <QLabel>
#include <QUuid>
#include <QMediaPlayer>
#include <QDir>
#include <QFile>
#include <QTimer>
#include <QDebug>

#include "avatarwidget.h"
#include "notifypanel.h"


const char *RedLed = "/sys/class/gpio/gpio295/value";
const char *YellowLed = "/sys/class/gpio/gpio296/value";
const char *BlueLed = "/sys/class/gpio/gpio296/value";

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
    text = new QLabel(this);
    hLayout->addWidget(text);

    QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    hLayout->addSpacerItem(spacer);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    connect(buttons, SIGNAL(accepted()), this, SLOT(confirm()));
    vLayout->addWidget(buttons);

    blinkIntervalMap[Low] = 3000;
    blinkIntervalMap[Middle] = 2000;
    blinkIntervalMap[High] = 1000;

    isOn = false;
    ledMap[Low] = BlueLed;
    ledMap[Middle] = YellowLed;
    ledMap[High] = RedLed;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(blink()));

    ledsOff();

    mediaMap[Low] = QString("%1/%2.mp3").arg(QDir::currentPath()).arg(QLatin1String("low"));
    mediaMap[Middle] = QString("%1/%2.mp3").arg(QDir::currentPath()).arg(QLatin1String("middle"));
    mediaMap[High] = QString("%1/%2.mp3").arg(QDir::currentPath()).arg(QLatin1String("high"));

    player = new QMediaPlayer(this);
    connect(player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(playerStateChanged(QMediaPlayer::State)));
}

void NotifyPanel::addNotify(Priority priority, const QString &text, const QString &icon)
{
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
    if (notifies) {
        notifies->append(QStringList() << QUuid::createUuid().toString() << text << icon);
        nextNotify();
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
    uuid = "";
    nextNotify();
    if (uuid.isEmpty()) {
        ledsOff();
        player->stop();
        accept();
    }
}

void NotifyPanel::blink()
{
    isOn = !isOn;
    setLedValue(ledMap[priority], isOn);
}

void NotifyPanel::playerStateChanged(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::StoppedState && !uuid.isEmpty()) {
        player->play();
    }
}

void NotifyPanel::nextNotify()
{
    if(uuid.isEmpty()) {
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

    uuid = notify[0];
    text->setText(notify[1]);
    avatar->setPixmap(QPixmap(notify[2]));
    if (!isVisible()) {
        exec();
    }
}

void NotifyPanel::setLedValue(const QString &led, bool on)
{
    QFile file(led);
    if (file.open(QIODevice::WriteOnly | QIODevice::Unbuffered)) {
        file.write(on ? "1" : "0");
        file.close();
    }
}

void NotifyPanel::ledsOff()
{
    timer->stop();
    isOn = false;
    setLedValue(RedLed, false);
    setLedValue(YellowLed, false);
    setLedValue(BlueLed, false);
}
