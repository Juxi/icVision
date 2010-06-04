#include "link.h"
#include "robot.h"
#include "displmatrix.h"
#include "cylinder.h"
#include <iostream>

using namespace std;

Link::Link( Robot* robot ) : parentLink(0),
                             parentRobot(robot),
                             index(robot->numLinks),
                             joint(0)
{
    initialize();
    robot->tree.append(this);
    robot->numLinks++;
}
Link::Link( Link* parentLink ) : parentLink(parentLink),
                                 parentRobot(parentLink->parentRobot),
                                 index(parentLink->parentRobot->numLinks),
                                 joint(0)
{
    initialize();
    parentLink->children.append(this);
    parentRobot->numLinks++;
}
Link::~Link()
{
    cout << "delete( Link" << index << " )..." << endl;
    QVector<Link*>::iterator i;
    for ( i=children.begin(); i!=children.end(); ++i ) {
        delete (*i);
    }
}
void Link::initialize()
{
    M.setToIdentity();
    T.setToIdentity();
    linkName.setNum(index);
    linkName.prepend("Link");
}

// GET STUFF
const int Link::idx() const { return index; }
const Robot* Link::robot() const { return parentRobot; }
Link* Link::parent() const { return parentLink; }
const QString&  Link::name() const { return linkName; }
const QMatrix4x4& Link::txfrToWorldCS() const { return T; }

// OTHER
void Link::emitFinished()
{
    parentRobot->emit finishLink( this );
}
void Link::filterCollisionPairs( DtResponse response )
{
    QVector<Link*>::iterator i;
    QVector<PhysObject*>::iterator j,k;
    for ( j=physObjectList.begin(); j!=physObjectList.end(); ++j ) {
        // ignore pairs where both objects are in this list
        for ( k=physObjectList.begin(); k!=physObjectList.end(); ++k ) {
            if ( j != k ) {
                dtSetPairResponse( (*j)->solid(), (*k)->solid(), response, DT_SIMPLE_RESPONSE, (void*) this );
                cout << "Filtered same-link pair on " << name().toStdString() << endl;
            }
        }
        for ( i=children.begin(); i!=children.end(); ++i ) {
            (*i)->filterAdjacentChildren( this, *j, response );
        }
    }
    for ( i=children.begin(); i!=children.end(); ++i ) {
        (*i)->filterCollisionPairs( response );
    }
}
void Link::filterAdjacentChildren( Link* link, PhysObject* obj, DtResponse response )
{
    QVector<PhysObject*>::iterator j;
    if ( !physObjectList.isEmpty() ) { // filter the objects in this list against those in link
        for ( j=physObjectList.begin(); j!=physObjectList.end(); ++j ) {
                //dtSetPairResponse( obj->solid(), (*j)->solid(), response, DT_SIMPLE_RESPONSE, (void*) this );
                dtClearPairResponse( obj->solid(), (*j)->solid() );
                cout << "Filtered pair [" << link->name().toStdString() << " " << name().toStdString() << "]!!" << endl;
        }
    } else { // recursively check children
        QVector<Link*>::iterator i;
        for ( i=children.begin(); i!=children.end(); ++i ) {
            (*i)->filterAdjacentChildren(link,obj, response);
        }
        cout << "Found child with empty objectList..." << endl;
    }
}

void Link::updateTxfrMatrix( const QMatrix4x4& TransformationMatrix )
{
    T = TransformationMatrix; // Save the Txfr matrix passed by the parent

    // update the physical objects that constitute this link
    QVector<PhysObject*>::iterator i;
    for ( i=physObjectList.begin(); i!=physObjectList.end(); ++i ) {
        (*i)->updateTxfrToWorld(T);
    }

    // update this link's children
    QMatrix4x4 nextT;
    if ( joint ) nextT = T * M * joint->getM() ;
    else nextT = T * M;
    QVector<Link*>::iterator j;
    for ( j=children.begin(); j!=children.end(); ++j ) {
        (*j)->updateTxfrMatrix(nextT);
    }
}
void Link::print() const
{
    const qreal* m = M.constData();
    const qreal* t = T.constData();
    //Alex: QDataStream??
    cout      << "      ---------------------------------------------------" << endl
              << "       Link " << index << ": " << linkName.toStdString() << endl
              << "      ---------------------------------------------------" << endl
              << endl
              << "      M:        " << m[0] << " " << m[4] << " " << m[8]  << " " << m[12] << endl
              << "                " << m[1] << " " << m[5] << " " << m[9]  << " " << m[13] << endl
              << "                " << m[2] << " " << m[6] << " " << m[10] << " " << m[14] << endl
              << "                " << m[3] << " " << m[7] << " " << m[11] << " " << m[15] << endl
              << "      T:        " << t[0] << " " << t[4] << " " << t[8]  << " " << t[12] << endl
              << "                " << t[1] << " " << t[5] << " " << t[9]  << " " << t[13] << endl
              << "                " << t[2] << " " << t[6] << " " << t[10] << " " << t[14] << endl
              << "                " << t[3] << " " << t[7] << " " << t[11] << " " << t[15] << endl
              << "      ---------------------------------------------------" << endl;
}
void Link::printAll()
{
    print();
    QVector<Link*>::iterator i;
    for ( i=children.begin(); i!=children.end(); ++i ) {
        (*i)->printAll();
    }
}
