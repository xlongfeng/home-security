#include "multipointcom.h"

MultiPointCom::MultiPointCom(QObject *parent) :
    QObject(parent)
{

}

void MultiPointCom::send(int id, const QByteArray &data)
{
    cache[id] = data;
}

void MultiPointCom::transmitToRemote()
{
    if (idle && !cache.isEmpty()) {
        idle = false;
        int id = cache.firstKey();
        const QByteArray &data = cache.take(id);
    }
}
