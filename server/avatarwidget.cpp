#include <QPixmap>
#include <QResizeEvent>
#include <QDebug>

#include "avatarwidget.h"

AvatarWidget::AvatarWidget(QWidget *parent) :
    QLabel(parent)
{
}

void AvatarWidget::setAvatar(const QPixmap &pixmap)
{
    avatar = pixmap;
    if (!avatar.isNull())
        setPixmap(avatar.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

QSize AvatarWidget::minimumSizeHint() const
{
    return QSize();
}

void AvatarWidget::resizeEvent(QResizeEvent *event)
{
    if (!avatar.isNull())
        setPixmap(avatar.scaled(event->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QLabel::resizeEvent(event);
}
