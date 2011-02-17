#ifndef WORLD_H
#define WORLD_H

#include <QObject>
#include <QVector>
#include "object.h"

class World : public QObject, public QVector<Object*>
{
    Q_OBJECT

public:
    World();
    ~World();

    void append( Object* object );
    //void filterCollisions();

signals:
    void addedObject( DisplayList* object );
    //void removedObject( Object* object );

};

#endif // WORLD_H
