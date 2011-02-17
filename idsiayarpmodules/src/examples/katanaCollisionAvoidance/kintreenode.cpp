#include "SOLID/solid.h"
#include "kintreenode.h"
#include "primitiveobject.h"
#include "robot.h"
#include <iostream>

using namespace std;

KinTreeNode::KinTreeNode( Robot* robot, Type type ) :  parentNode(0),
                                                       parentRobot(robot),
                                                       index(robot->nextLinkIdx()),
                                                       nodeType(type)
{
    nodeName.setNum(index);
    nodeName.prepend("Node");
    parentRobot->appendLink(this);
}
KinTreeNode::KinTreeNode( KinTreeNode* parent, Type type ) : parentNode(parent),
                                                             parentRobot(parent->robot()),
                                                             index(parent->robot()->nextLinkIdx()),
                                                             nodeType(type)
{
    nodeName.setNum(index);
    nodeName.prepend("Node");
    parentNode->children.append(this);
}
KinTreeNode::~KinTreeNode()
{
}
bool KinTreeNode::setAxis( const QVector3D& vector )
{
    if ( vector.length() > 0 ) {
        nodeAxis = vector;
        setM();
        return true;
    } else return false;
}
void KinTreeNode::emitFinished()
{
    parentRobot->emit finishLink( &this->object );  // passing a pointer vs a const reference???
}
void KinTreeNode::filterCollisionPairs()
{
    QVector<KinTreeNode*>::iterator i;

    // do not do collision detection between this link and its siblings (or itself)
    if ( !parentNode ) {
        for ( i=parentRobot->tree.begin(); i!=parentRobot->tree.end(); ++i ) {
            //doNotCheckCollision(this,*i);
            serialFilter( *i );
        }
    } else {
        for ( i=parentNode->children.begin(); i!=parentNode->children.end(); ++i ) {
            //doNotCheckCollision(this,*i);
            serialFilter( *i );
        }
    }

    for ( i=children.begin(); i!=children.end(); ++i ) {
        (*i)->filterCollisionPairs();
    }
}

void KinTreeNode::serialFilter( KinTreeNode* node, bool foundLink, bool foundJoint )
{
    // to control recursion down serial chains
    if ( this->parent() != node->parent() ) {
        if ( foundLink && foundJoint ) return;

        if ( type() == LINK && parent()->type() != LINK ) {
            foundLink = true;
        }
        else if ( type() != LINK && parent()->type() == LINK ) {
            foundJoint = true;
        }
    }

    // for debugging
    cout << "Filtering " << node->name().toStdString() << ", " << this->name().toStdString() << endl;

    // do not check collision between this object and the one in 'node'
    object.doNotCheckCollision(node->getObject());

    // do not check collision against any siblings of node (also down serial chain)
    QVector<KinTreeNode*>::iterator i = 0;
    bool foundNode = false;
    if ( !node->parent() ) {
        for ( i=node->parentRobot->tree.begin(); i!=node->parentRobot->tree.end(); ++i ) {
            if ( foundNode ) (*i)->serialFilter(this);
            if ( *i == node ) foundNode = true;
        }
    } else {
        for ( i=node->parent()->children.begin(); i!=node->parent()->children.end(); ++i ) {
            if ( foundNode ) (*i)->serialFilter(this);
            if ( *i == node ) foundNode = true;
        }
    }


    QVector<KinTreeNode*>::iterator j,k;
    for ( j=children.begin(); j!=children.end(); ++j ) {
        (*j)->serialFilter(node,foundLink,foundJoint);
        (*j)->serialFilter(*j);
    }
}

void KinTreeNode::updatePosition( const QMatrix4x4& txfr )
{
	//cout << " updating position Node" << idx() << endl;
    // update the physical objects that constitute this link
    object.setT( txfr );

    // update this link's children
    QMatrix4x4 nextT = txfr * M;
    QVector<KinTreeNode*>::iterator j;
    for ( j=children.begin(); j!=children.end(); ++j ) {
        (*j)->updatePosition(nextT);
    }
}

void KinTreeNode::print() const
{
    const qreal* m = M.constData();
    const qreal* t = nextT.constData();
    //Alex: QDataStream??
         cout << "   ---------------------------------------------------" << endl
              << "    Node " << index << ": " << nodeName.toStdString()   << endl
              << "   ---------------------------------------------------" << endl
              << "      Axis:     " << nodeAxis.x() << " " << nodeAxis.y() << " " << nodeAxis.z() << endl
              << "      M:        " << m[0] << " " << m[4] << " " << m[8]  << " " << m[12] << endl
              << "                " << m[1] << " " << m[5] << " " << m[9]  << " " << m[13] << endl
              << "                " << m[2] << " " << m[6] << " " << m[10] << " " << m[14] << endl
              << "                " << m[3] << " " << m[7] << " " << m[11] << " " << m[15] << endl
              << "      T:        " << t[0] << " " << t[4] << " " << t[8]  << " " << t[12] << endl
              << "                " << t[1] << " " << t[5] << " " << t[9]  << " " << t[13] << endl
              << "                " << t[2] << " " << t[6] << " " << t[10] << " " << t[14] << endl
              << "                " << t[3] << " " << t[7] << " " << t[11] << " " << t[15] << endl
              << "   ---------------------------------------------------" << endl;
}
void KinTreeNode::printAll()
{
    print();
    QVector<KinTreeNode*>::iterator i;
    for ( i=children.begin(); i!=children.end(); ++i ) {
        (*i)->printAll();
    }
}
