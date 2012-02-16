#include <SOLID.h>
#include "kintreenode.h"
#include "primitiveobject.h"
#include "robot.h"
#include "modelexception.h"
#include "model.h"

#include <QThread>

using namespace KinematicModel;

KinTreeNode::KinTreeNode( //DT_RespTableHandle robotTable,	//DT_ResponseClass nodeResponseClass,
						  Robot* robot, 
						  KinTreeNode* parent,
						  NodeType aType ) :	CompositeObject( robot->model->newResponseClass(robot->getResponseTable()), robot->getResponseTable() ),
												parentRobot(robot),
												parentNode(parent),
												//index(robot->numNodes()),
												nodeType(aType)
{
	if ( !robot ) { throw KinematicModelException("The KinTreeNode constructor requires a pointer to a valid Robot."); }
	
	//obj = new CompositeObject( model->ROBOT() );
    //objectName.setNum(index);
    //objectName.prepend("BodyPart");
	//setObjectType(CompositeObject::BODY_PART);
    
	if ( !parentNode ) { parentRobot->appendNode(this); }
	else { parentNode->children.append(this); }
	
	//parentRobot->emit appendedObject(this);
	//parentRobot->emit requestDisplayList(static_cast<DisplayList*>(this));
}

KinTreeNode::~KinTreeNode()
{
	/*QVector<KinTreeNode*>::iterator i;
    for ( i=children.begin(); i!=children.end(); ++i ) {
        delete(*i);
    }
	
	QVector<PrimitiveObject*>::iterator j;
	for ( j=begin(); j!=end(); ++j ) { parentRobot->emit outdatedDisplayList( (*j)->displayListIdx() ); }
	
	parentRobot->emit outdatedDisplayList( displayListIdx() );*/
}

void KinTreeNode::kill()
{
    QVector<KinTreeNode*>::iterator i;
    for ( i=children.begin(); i!=children.end(); ++i ) {
        (*i)->kill();
    }
	CompositeObject::kill();
}

void KinTreeNode::setNodeAxis( const QVector3D& vector )
{
    if ( vector.length() <= 0 ) { throw KinematicModelException("Axis must have non zero magnitude (set x,y or z > 0)"); }
	nodeAxis = vector;
	setM();
}

//void KinTreeNode::append( PrimitiveObject* primitive )
//{ 
	//CompositeObject::append(primitive);
	//parentRobot->emit appendedPrimitive(primitive);
	//parentRobot->emit requestDisplayList(static_cast<DisplayList*>(primitive));
//}

/*bool KinTreeNode::remove( PrimitiveObject* primitive )
{ 
	//parentRobot->emit outdatedDisplayList( primitive->displayListIdx() );
	//return CompositeObject::remove(primitive);
}*/

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

bool KinTreeNode::isNearRoot( KinTreeNode* node, bool foundLink, bool foundJoint )
{
	if ( !node ) node = this;
	
	// to control recursion up serial chains
	if ( !parent() ) 
	{
		printf("isNearRoot returns TRUE\n");
		return true;
	}

        if ( foundLink && foundJoint && getNodeType() != node->getNodeType() ) // TODO: handle PJOINT
		{
			printf("isNearRoot returns FALSE\n");
			return false;
		}
        if ( getNodeType() == LINK && parent()->getNodeType() != LINK && !data().isEmpty() )
		{
            foundLink = true;
        }
        else if ( getNodeType() != LINK && parent()->getNodeType() == LINK && !data().isEmpty() )
		{
            foundJoint = true;
        }

	
    return parent()->isNearRoot(node,foundLink,foundJoint);
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
        if ( getNodeType() == LINK && parent()->getNodeType() != LINK && !data().isEmpty() )
		{
            foundLink = true;
        }
        else if ( getNodeType() != LINK && parent()->getNodeType() == LINK && !data().isEmpty() )
		{
            foundJoint = true;
        }
    }

    // do not check collision between objects of the same class as this one and objects of the same class as 'node'
	parentRobot->model->removePairResponse( parentRobot->responseTable, getResponseClass(), node->getResponseClass() );
	
	
	

    QVector<KinTreeNode*>::iterator i = 0;
    for ( i=children.begin(); i!=children.end(); ++i ) {
        (*i)->serialFilter(node,foundLink,foundJoint);
    }
}
/*
void KinTreeNode::render()
{ 
		//CompositeObject::render();
		QVector<KinTreeNode*>::iterator j;
		for ( j=children.begin(); j!=children.end(); ++j ) {
			(*j)->render();
		}
}*/

void KinTreeNode::update( const QMatrix4x4& txfr )
{
	//printf("Doing Fwd Kin - %p\n",QThread::currentThread());
	
	// update the physical objects that constitute this link
	setT( txfr );
	
	QVector<KinTreeNode*>::iterator j;
	for ( j=children.begin(); j!=children.end(); ++j ) {
		(*j)->update( txfr * M.getT() );
	}
}

int KinTreeNode::getNumPrimitives()
{
	int result = primitives.size();
	QVector<KinTreeNode*>::iterator i;
    for ( i=children.begin(); i!=children.end(); ++i ) {
        result += (*i)->getNumPrimitives();
    }
	return result;
}

/*
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
}*/

/*bool KinTreeNode::isColliding()
{
	if ( CompositeObject::isColliding() )
	{ 
		return true;
	}
	QVector<KinTreeNode*>::iterator i;
    for ( i=children.begin(); i!=children.end(); ++i )
	{
        if ( (*i)->isColliding() )
		{
			return true;
		}
    }
	return false;
}*/
