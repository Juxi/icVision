#include "world.h"
#include <iostream>

using namespace std;

World::World()
{
}
World::~World()
{
}
void World::append( Object* object )
{
    QVector<Object*>::append( object );
    QVector<Object*>::iterator i;
    for ( i=begin(); i!=end(); ++i ) {
        object->doNotCheckCollision(*i);
    }
    emit addedObject( object );
    //cout << "emitted world signal" << endl;
}
