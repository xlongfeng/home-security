#ifndef AVATARWIDGET_H
#define AVATARWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>

class AvatarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AvatarWidget(QWidget *parent = 0);

    void setPixmap(const QPixmap &pixmap)
    {
        this->pixmap = pixmap;
    }

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    QPixmap pixmap;
};

#endif // AVATARWIDGET_H
