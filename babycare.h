#ifndef BABYCARE_H
#define BABYCARE_H

#include <QGraphicsView>

class BabyCare : public QGraphicsView
{
    Q_OBJECT

public:
    BabyCare(QWidget *parent = 0);

protected:
    virtual void resizeEvent(QResizeEvent *event);

private:
    QGraphicsScene *sense;
    QGraphicsPixmapItem *mapItem;
    QPixmap map;
};

#endif // BABYCARE_H
