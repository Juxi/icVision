#ifndef KINTREENODE_H
#define KINTREENODE_H

#include <QMutex>
#include <QtGui>
#include "object.h"

class PrimitiveObject;
class Robot;
//#include "robot.h"

class KinTreeNode : public CompositeObject
{
public:
	
	enum Type {
				LINK,
				RJOINT,
				PJOINT
	};

    KinTreeNode( Robot* parentRobot, KinTreeNode* parentNode, Type type = LINK );
    //KinTreeNode( KinTreeNode* parent, Type type = LINK );
    virtual ~KinTreeNode();

    // set stuff
    //void setNodeName( const QString& name ) { nodeName = name; }
    void setNodeAxis( const QVector3D& vector );

    // get stuff
    KinTreeNode* parent() const { return parentNode; }
    Robot* robot() const { return parentRobot; }
    const int idx() const { return index; }
	
    //const QString& getNodeName() const { return nodeName; }
    const Type& getNodeType() const { return nodeType; }
    const QVector3D& getNodeAxis() const { return nodeAxis; }
    const QMatrix4x4& getM() const { return M; } // to transform children relative to this node coordinate system

	//append geometries
	void append( PrimitiveObject* primitive );
	bool remove( PrimitiveObject* primitive );
	
	void render();

    void filterCollisionPairs();
    void serialFilter( KinTreeNode* node, bool link = false, bool joint = false );
	
    void update( const QMatrix4x4& txfr );
    void notColliding();
	
	void print() const;
    void printAll();

protected:
    Robot*                parentRobot;
	KinTreeNode*          parentNode;
    int                   index;
    //QString               nodeName;     // a user-readable identifier for the joint (no uniqueness check)
    Type                  nodeType;
    //Object                object;       // for SOLID and OpenGL...  is there a better way?
    QVector<KinTreeNode*> children;
    QVector3D             nodeAxis;     // joint axis or link body vector
    DisplMatrix           M;            // transformation (displacement and rotation) matrix from the next CS to this CS
	QMutex				  mutex;

    virtual void setM() = 0;

};

#endif // KINTREENODE_H
