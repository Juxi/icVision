#include "object.h"
#include "primitiveobject.h"
#include <iostream>

using namespace std;

Object::Object( Object* parent ) : parentObject(parent)
{
}
Object::~Object()
{
}
void Object::setT( const QMatrix4x4& txfr )
{
    DisplayList::setT(txfr);
    QVector<PrimitiveObject*>::iterator i;
    for (i=begin(); i!=end(); ++i) {
        (*i)->setT(txfr);
    }
}
void Object::append(PrimitiveObject *primitiveObject)
{
    QVector<PrimitiveObject*>::append( primitiveObject );
    primitiveObject->setParent(this);
}
void Object::doNotCheckCollision( const Object* a ) {
    doNotCheckCollision(a,this);
}
void Object::doNotCheckCollision( const Object* a, const Object* b )
{
    QVector<PrimitiveObject*>::const_iterator i,j;
    for ( i=a->begin(); i!=a->end(); ++i ) {
        for ( j=b->begin(); j!=b->end(); ++j ) {
                dtClearPairResponse( *i, *j );
        }
    }
}
void Object::makeDisplayList()
{

    // get a unique display list index and define the list
    setDisplayListIdx(glGenLists(10000));

    glNewList( displayListIdx(), GL_COMPILE );  // a cylinder expressed in a centroidal y-principal CS

        //qreal csLen = 1.5*radius;
        GLfloat red[] = {1,0,0,1};
        GLfloat green[] = {0,1,0,1};
        GLfloat blue[] = {0,0,1,1};
        glBegin(GL_LINES);
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
            glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, red);
            glVertex3f(0,0,0); glVertex3f(1,0,0);

            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);
            glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, green);
            glVertex3f(0,0,0); glVertex3f(0,1,0);

            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, blue);
            glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, blue);
            glVertex3f(0,0,0); glVertex3f(0,0,1);
        glEnd();

    glEndList();
    cout << "made object display list" << endl;

    // make display lists for primitives
    QVector<PrimitiveObject*>::const_iterator i;
    for ( i=begin(); i!=end(); ++i ) {
        (*i)->makeDisplayList();
    }
}

void Object::render()
{
    DisplayList::render();
    // make display lists for primitives
    QVector<PrimitiveObject*>::const_iterator i;
    for ( i=begin(); i!=end(); ++i ) {
        (*i)->render();
    }
}
