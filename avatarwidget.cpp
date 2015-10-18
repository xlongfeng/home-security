#include <QPixmap>
#include <QStyleOption>

#include "avatarwidget.h"

AvatarWidget::AvatarWidget(QWidget *parent) :
    QWidget(parent)
{
}

void AvatarWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    if (pixmap.isNull())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    QSize pixmapSize = pixmap.size();
    pixmapSize.scale(event->rect().size(), Qt::KeepAspectRatio);

    QPoint topleft;
    topleft.setX((this->width() - pixmapSize.width()) / 2);
    topleft.setY((this->height() - pixmapSize.height()) / 2);

    painter.drawPixmap(topleft, pixmap.scaled(pixmapSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
