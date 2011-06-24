#include <SOLID.h>
#include "kintreenode.h"
#include "primitiveobject.h"
#include "robot.h"
#include "robotmodelexception.h"
#include "model.h"

#include <QThread>

using namespace RobotModel;

KinTreeNode::KinTreeNode( Robot* robot, KinTreeNode* parent, NodeType aType ) :  parentRobot(robot),
																		    parentNode(parent),
																		    index(robot->numNodes()),
																		    nodeType(aType)
{
	if ( !robot ) { throw RobotModelException("The KinTreeNode constructor requires a pointer to a valid Robot."); }
	
    objectName.setNum(index);
    objectName.prepend("BodyPart");
	setObjectType(CompositeObject::BODY_PART);
	solidObjectType = DT_GenResponseClass(parentRobot->model->getRobotTable());
    
	if ( !parentNode ) { parentRobot->appendNode(this); }
	else { parentNode->children.append(this); }
	
	parentRobot->emit appendedObject(static_cast<DisplayList*>(this));
}

KinTreeNode::~KinTreeNode()
{
	QVector<KinTreeNode*>::iterator i;
    for ( i=children.begin(); i!=children.end(); ++i ) {
        delete(*i);
    }
	
	QVector<PrimitiveObject*>::iterator j;
	for ( j=begin(); j!=end(); ++j ) { parentRobot->emit outdatedDisplayList( (*j)->displayListIdx() ); }
	
	parentRobot->emit outdatedDisplayList( displayListIdx() );
}

void KinTreeNode::setNodeAxis( const QVector3D& vector )
{
    if ( vector.length() <= 0 ) { throw RobotModelException("Axis must have non zero magnitude (set x,y or z > 0)"); }
	nodeAxis = vector;
	setM();
}

void KinTreeNode::append( PrimitiveObject* anObject )
{ 

	DT_AddObject( parentRobot->model->getScene(), anObject->getSolidHandle() );
	DT_SetResponseClass( parentRobot->model->getRobotTable(), anObject->getSolidHandle(), solidObjectType );
	DT_SetResponseClass( parentRobot->model->getWorldTable(), anObject->getSolidHandle(), parentRobot->model->body_partRespClass() );
	CompositeObject::append(anObject);
	parentRobot->emit appendedObject(static_cast<DisplayList*>(anObject));
}

bool KinTreeNode::remove( PrimitiveObject* anObject )
{ 
	parentRobot->emit outdatedDisplayList( anObject->displayListIdx() );
	return CompositeObject::remove(anObject);
}

void KinTreeNode::filterCollisionPairs()
{
    QVector<KinTreeNode*>::iterator i;

    // do not do collision detection between this link and its siblings (or itself)
    if ( !parentNode )
	{
        for ( i=parentRobot->tree.begin(); i!=parentRobot->tree.end(); ++i ) serialFilter( *i );
    } else {
        for ( i=parentNode->children.begin(); i!=parentNode->children.end(); ++i ) serialFilter( *i );
    }

    for ( i=children.begin(); i!=children.end(); ++i ) {
        (*i)->filterCollisionPairs();
    }
}

void KinTreeNode::serialFilter( KinTreeNode* node, bool foundLink, bool foundJoint )
{
    // to control recursion down serial chains
    if ( this->parent() && this->parent() != node->parent() )
	{
        if ( foundLink && foundJoint && getNodeType() != node->getNodeType() ) // TODO: handle PJOINT
		{
			return;
		}
        if ( getNodeType() == LINK && parent()->getNodeType() != LINK && !isEmpty() )
		{
            foundLink = true;
        }
        else if ( getNodeType() != LINK && parent()->getNodeType() == LINK && !isEmpty() )
		{
            foundJoint = true;
        }
    }

    // do not check collision between objects of the same class as this one and objects of the same class as 'node'
    parentRobot->model->removeSelfCollisionPair( solidObjectType, node->getSolidType() );

    QVector<KinTreeNode*>::iterator i = 0;
    for ( i=children.begin(); i!=children.end(); ++i ) {
        (*i)->serialFilter(node,foundLink,foundJoint);
    }
}

void KinTreeNode::render()
{ 
		CompositeObject::render();
		QVector<KinTreeNode*>::iterator j;
		for ( j=children.begin(); j!=children.end(); ++j ) {
			(*j)->render();
		}
}

void KinTreeNode::update( const QMatrix4x4& txfr )
{
	//printf("Doing Fwd Kin - %p\n",QThread::currentThread());
	
	// update the physical objects that constitute this link
	setT( txfr );

	// update this link's children
	QMatrix4x4 nextT = txfr * M;
	QVector<KinTreeNode*>::iterator j;
	for ( j=children.begin(); j!=children.end(); ++j ) {
		(*j)->update(nextT);
	}
}

int KinTreeNode::getNumPrimitives()
{
	int result = size();
	QVector<KinTreeNode*>::iterator i;
    for ( i=children.begin(); i!=children.end(); ++i ) {
        result += (*i)->getNumPrimitives();
    }
	return result;
}

void KinTreeNode::print() const
{
    const qreal* m = M.constData();
    const qreal* t = T.constData();
   
	printf("   ---------------------------------------------------\n");
	printf("     Node: %d\n", index);
	printf("   ---------------------------------------------------\n");
	printf("       Axis: %f, %f, %f\n", nodeAxis.x(),nodeAxis.y(),nodeAxis.z());
	printf("       M: %f, %f, %f, %f\n",	m[0],	m[4],	m[8],	m[12]);
	printf("          %f, %f, %f, %f\n",	m[1],	m[5],	m[9],	m[13]);
	printf("          %f, %f, %f, %f\n",	m[2],	m[6],	m[10],	m[14]);
	printf("          %f, %f, %f, %f\n",	m[3],	m[7],	m[11],	m[15]);
	printf("       T: %f, %f, %f, %f\n",	t[0],	t[4],	t[8],	t[12]);
	printf("          %f, %f, %f, %f\n",	t[1],	t[5],	t[9],	t[13]);
	printf("          %f, %f, %f, %f\n",	t[2],	t[6],	t[10],	t[14]);
	printf("          %f, %f, %f, %f\n",	t[3],	t[7],	t[11],	t[15]);
	printf("   ---------------------------------------------------\n");
}

void KinTreeNode::printAll()
{
    print();
    QVector<KinTreeNode*>::iterator i;
    for ( i=children.begin(); i!=children.end(); ++i ) {
        (*i)->printAll();
    }
}
