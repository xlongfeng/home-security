#include <QApplication>
#include <QGraphicsPixmapItem>
#include <QResizeEvent>
#include <QDebug>

#include "babycare.h"


BabyCare::BabyCare(QWidget *parent) :
    QGraphicsView(parent)
{
    setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);

    sense = new QGraphicsScene(this);
    setScene(sense);

    map = QPixmap(qApp->applicationDirPath() + "/images/home.png");
    mapItem = sense->addPixmap(QPixmap(1, 1));
    sense->addText("hello world");
}

void BabyCare::resizeEvent(QResizeEvent *event)
{
    setSceneRect(QRect(QPoint(0, 0), event->size()));
    mapItem->setPixmap(map.scaled(event->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    QGraphicsView::resizeEvent(event);
}
