#ifndef KINTREENODE_H
#define KINTREENODE_H

#include <QtGui>
#include "object.h"

class PrimitiveObject;
class Robot;
//#include "robot.h"

class KinTreeNode
{
public:
    enum Type {
          LINK,
          RJOINT,
          PJOINT
       };

    KinTreeNode( Robot* parent, Type type = LINK );
    KinTreeNode( KinTreeNode* parent, Type type = LINK );
    virtual ~KinTreeNode();

    // set stuff
    void setName( const QString& name ) { nodeName = name; }
    void appendToObject( PrimitiveObject* anObject ) { object.append(anObject); }
    bool setAxis( const QVector3D& vector );

    // get stuff
    KinTreeNode* parent() const { return parentNode; }
    Robot* robot() const { return parentRobot; }
    const int idx() const { return index; }
    const QString& name() const { return nodeName; }
    const Type& type() const { return nodeType; }
    const QVector3D& axis() const { return nodeAxis; }
    const QMatrix4x4& getM() const { return M; } // to transform children relative to this node coordinate system
    const QMatrix4x4& getT() const { return nextT; } // transformation from all parents (displaces this node to its 3-space position)
    const QMatrix4x4& rGetT() { nextT = parentNode->rGetT()*M; return nextT; }
    const Object* getObject() const { return &object; }

    void print() const;
    void printAll();

    void emitFinished();
    void filterCollisionPairs();
    void serialFilter( KinTreeNode* node, bool link = false, bool joint = false );
    void updatePosition( const QMatrix4x4& txfr );

protected:
    KinTreeNode*          parentNode;
    Robot*                parentRobot;
    int                   index;
    QString               nodeName;     // a user-readable identifier for the joint (no uniqueness check)
    Type                  nodeType;
    Object                object;       // for SOLID and OpenGL...  is there a better way?
    QVector<KinTreeNode*> children;
    QVector3D             nodeAxis;     // joint axis or link body vector
    QMatrix4x4            M,            // transformation (displacement and rotation) matrix from the next CS to this CS
                          nextT;            // transformation (displacement and rotation) matrix from the next CS to WCS

    virtual void setM() = 0;


};

#endif // KINTREENODE_H
