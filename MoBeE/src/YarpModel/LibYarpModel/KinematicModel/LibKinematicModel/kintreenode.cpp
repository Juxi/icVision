#include <SOLID.h>
#include "kintreenode.h"
#include "primitiveobject.h"
#include "robot.h"
#include "modelexception.h"
#include "model.h"

#include <QThread>

using namespace KinematicModel;

KinTreeNode::KinTreeNode( Robot* robot,
                         int part,
						  KinTreeNode* parent,
						  NodeType aType ) :	CompositeObject( robot->model->newResponseClass(robot->getResponseTable()),
                                                                 robot->model->newResponseClass(robot->getFieldResponseTable())),
												parentRobot(robot),
                                                bodyPart(part),
												parentNode(parent),
												nodeType(aType)
												//strf(true)
{
	if ( !robot ) { throw KinematicModelException("The KinTreeNode constructor requires a pointer to a valid Robot."); }
	if ( !parentNode ) { parentRobot->appendNode(this); }
	else { parentNode->children.append(this); }
    
    // kintreenosed are repulsive hahaha!
    //fieldResponseClass = robot->model->newResponseClass(robot->getResponseTable());
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

/*void KinTreeNode::removeCollisionResponse( DT_ResponseClass c, DT_RespTableHandle t )
{
	QVector<KinTreeNode*>::iterator i;
    for ( i=children.begin(); i!=children.end(); ++i ) {
        (*i)->removeCollisionResponse();
    }
}*/

void KinTreeNode::ignoreAdjacentPairs()
{
    QVector<KinTreeNode*>::iterator i;
	
    // do not do collision detection between this link and its siblings (or itself)
    if ( !parentNode )
	{
        for ( i=parentRobot->tree.begin(); i!=parentRobot->tree.end(); ++i ) 
			ignoreAdjacentPairs( *i );
    } else
	{
        for ( i=parentNode->children.begin(); i!=parentNode->children.end(); ++i )
			ignoreAdjacentPairs( *i );
    }
	
    for ( i=children.begin(); i!=children.end(); ++i )
	{
        (*i)->ignoreAdjacentPairs();
    }
}

void KinTreeNode::ignoreAdjacentPairs( KinTreeNode* node, bool foundLink, bool foundJoint )
{
    // to control recursion down serial chains
    if ( this->parent() && this->parent() != node->parent() )
	{
        if ( foundLink && foundJoint && getNodeType() != node->getNodeType() ) // TODO: handle PJOINT
			return;
        if ( getNodeType() == LINK && parent()->getNodeType() != LINK && !data().isEmpty() )
            foundLink = true;
        else if ( getNodeType() != LINK && parent()->getNodeType() == LINK && !data().isEmpty() )
            foundJoint = true;
    }
	
    // filter the collision if nodes are in the same body part
    bool filter = false;
    if ( bodyPart == node->getBodyPartIndex() || node == parent() || node->parent() == parent() )
        filter = true;
    //else if ( parent() ) {
    //    if ( node == parent()->parent() || node->parent() == )
    //    filter = true;
    //}
    //else if ( parent() && parent()->getBodyPartIndex() != getBodyPartIndex() )
    //    filter = true;
    //else if ( parent() && parent()->parent() && parent()->parent()->getBodyPartIndex() != parent()->getBodyPartIndex() )
    //    filter = true;
    else
        filter = false;
    
    if ( filter )
    {
        // do not check collision between objects of the same class as this one and objects of the same class as 'node'
        parentRobot->getModel()->removeAllResponses( parentRobot->responseTable, getResponseClass(), node->getResponseClass() );
        parentRobot->getModel()->removeAllResponses( parentRobot->responseTable, getFieldResponseClass(), node->getFieldResponseClass() );
	}
    
    QVector<KinTreeNode*>::iterator i = 0;
    for ( i=children.begin(); i!=children.end(); ++i ) {
        (*i)->ignoreAdjacentPairs(node,foundLink,foundJoint);
    }
}

void KinTreeNode::removeReflexFromSubTree()
{
	
    // turn off reflex response between elements of the sub-tree originating from this node
	QVector<KinTreeNode*> nodeList;
	getSubTree(nodeList);
	QVector<KinTreeNode*>::iterator i,j;
	
	for ( i=nodeList.begin(); i!=nodeList.end(); ++i )
	{
		for ( j=nodeList.begin(); j!=nodeList.end(); ++j )
		{
			parentRobot->getModel()->removeReflexResponse( parentRobot->responseTable, (*i)->getResponseClass(), (*j)->getResponseClass() );
			//parentRobot->getModel()->setVisualResponse( parentRobot->responseTable, (*i)->getResponseClass(), (*j)->getResponseClass() );
			//parentRobot->model->removePairResponses( parentRobot->responseTable, (*i)->getResponseClass(), (*j)->getResponseClass() );
		}
	}
}

void KinTreeNode::getSubTree( QVector<KinTreeNode*>& nodeList )
{
	QVector<KinTreeNode*>::iterator i;
	for ( i=children.begin(); i!=children.end(); ++i )
        (*i)->getSubTree(nodeList);
	nodeList.append(this);
}

bool KinTreeNode::isNearRoot( KinTreeNode* node, bool foundLink, bool foundJoint )
{
	if ( !node ) node = this;
	
	// to control recursion up serial chains
	if ( !parent() ) 
		return true;
	
	if ( foundLink && foundJoint && getNodeType() != node->getNodeType() ) // TODO: handle PJOINT
		return false;
	
	if ( getNodeType() == LINK && parent()->getNodeType() != LINK /*&& !data().isEmpty()*/ )
		foundLink = true;
	else if ( getNodeType() != LINK && parent()->getNodeType() == LINK /*&& !data().isEmpty()*/ )
		foundJoint = true;
	
    return parent()->isNearRoot(node,foundLink,foundJoint);
}

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


QList< QPair<QVector3D, QVector3D> > KinTreeNode::computeJacobian()
{
    QList< QPair<QVector3D, QVector3D> > J;
    QVector4D p = getT()*QVector4D(0,0,0,1);
    
    KinTreeNode* up = this;
    while (up->parent())
    // TODO: While not root of body part
    {
        up = up->parent();
        if ( up->getNodeType() == RJOINT )
        {
            QVector4D q = up->getT()*QVector4D(0,0,0,1);
            QVector3D qp = (p-q).toVector3D();
            QVector3D jointAxis = up->getNodeAxis().normalized();
            
            // component of qp orthogonal to the joint axis
            //qp -= QVector3D::dotProduct(qp,jointAxis)*jointAxis;
            // make a column of the jacobian
            //QVector3D F = qp.length() * QVector3D::crossProduct(jointAxis, qp);
            
            J.prepend(
                     QPair<QVector3D, QVector3D>(
                                                  QVector3D::crossProduct(jointAxis, qp),
                                                  jointAxis
                                                )
                    );
        }
    }
    return J;
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
