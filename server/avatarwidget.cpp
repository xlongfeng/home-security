#include <QPixmap>
#include <QStyleOption>
#include <QRadialGradient>
#include <QDebug>

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

#if 0
    const QRect &rect = event->rect();
    const int w = rect.width();
    const int h = rect.height();
    int sideLength = w < h ? w : h;
    int x = w < h ? 0 : ((w - h) / 2);
    int y = h < w ? 0 : ((h - w) / 2);
    QRect square(x, y, sideLength, sideLength);

    qDebug() << square;

    QRadialGradient gradient(square.center(), sideLength);
    gradient.setColorAt(0.5, Qt::green);
    gradient.setColorAt(1, Qt::black);
    painter.fillRect(square, gradient);
    pixmapSize.scale(event->rect().size() - QSize(8, 8), Qt::KeepAspectRatio);
#else
    pixmapSize.scale(event->rect().size(), Qt::KeepAspectRatio);
#endif

    QPoint topleft;
    topleft.setX((this->width() - pixmapSize.width()) / 2);
    topleft.setY((this->height() - pixmapSize.height()) / 2);

    painter.drawPixmap(topleft, pixmap.scaled(pixmapSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
