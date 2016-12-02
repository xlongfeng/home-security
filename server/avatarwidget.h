#ifndef AVATARWIDGET_H
#define AVATARWIDGET_H

#include <QLabel>

class AvatarWidget : public QLabel
{
    Q_OBJECT
public:
    explicit AvatarWidget(QWidget *parent = 0);
    void setAvatar(const QPixmap &pixmap);

    virtual QSize minimumSizeHint() const;

protected:
    void resizeEvent(QResizeEvent *event);

private:
    QPixmap avatar;

};

#endif // AVATARWIDGET_H
