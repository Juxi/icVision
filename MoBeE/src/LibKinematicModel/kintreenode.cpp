#include <SOLID.h>
#include "kintreenode.h"
#include "joint_revolute.h"
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

const QVector3D KinTreeNode::getCurrentNodeAxis()
{
    QVector4D zeroPositionAxis = nodeAxis.normalized().toVector4D();
    return (getT()*zeroPositionAxis).toVector3D();
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
        parentRobot->getModel()->removeAllResponses( parentRobot->getResponseTable(), getResponseClass(), node->getResponseClass() );
	}
    
    // remove field responses even across body parts
    parentRobot->getModel()->removeAllResponses( parentRobot->getFieldResponseTable(), getFieldResponseClass(), node->getFieldResponseClass() );
    
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
			parentRobot->getModel()->removeReflexResponse( parentRobot->getResponseTable(), (*i)->getResponseClass(), (*j)->getResponseClass() );
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


/*QList< QPair<QVector3D, QVector3D> > KinTreeNode::computeJacobian( QVector3D F, QVector3D T )
{
    QList< QPair<QVector3D, QVector3D> > J;
    QVector3D p = getPos();
    
    KinTreeNode* up = this;
    RevoluteJoint* joint = NULL;
    
    while (up->parent())
    {
        up = up->parent();
        if ( up->getNodeType() == RJOINT )
        {
            joint = dynamic_cast<KinematicModel::RevoluteJoint*>(up);
            if (joint) {
                QVector3D q = up->getPos();
                QVector3D qp = p-q;
                QVector3D jointAxis = up->getCurrentNodeAxis();
                
                // component of qp orthogonal to the joint axis
                //qp -= QVector3D::dotProduct(qp,jointAxis)*jointAxis;
                // make a column of the jacobian
                //QVector3D F = qp.length() * QVector3D::crossProduct(jointAxis, qp);
                
                QPair<QVector3D, QVector3D> jColumn( QVector3D::crossProduct(jointAxis, qp), jointAxis );
                
                J.prepend(jColumn);
                
                if ( joint->isBodyPartRoot() )
                    break;
            }
        }
        
    }
    return J;
}*/

QList< QPair<QVector3D, QVector3D> > KinTreeNode::computeJacobian( QVector3D F, QVector3D T )
{
    QList< QPair<QVector3D, QVector3D> > J;
    QVector3D p = getPos();
    
    KinTreeNode* thisNode = this;
    
    while (true)
    {
        //printf("entered computeJacobian()\n");
        if ( thisNode->getNodeType() == RJOINT )
        {
            //printf("found a joint\n");
            RevoluteJoint* joint = dynamic_cast<KinematicModel::RevoluteJoint*>(thisNode);
            if (joint) {
                QVector3D q = joint->getPos();
                QVector3D qp = p-q;
                QVector3D jointAxis = joint->getCurrentNodeAxis();
       
                QPair<QVector3D, QVector3D> jColumn( QVector3D::crossProduct(jointAxis, qp), jointAxis );
                
                // project F and T onto this joint
                qreal jointTorque = QVector3D::dotProduct(jColumn.first, F) + QVector3D::dotProduct(jColumn.second, T);
                joint->parentMotor()->addTorque(jointTorque);
                
                //printf("jColumn: %f, %f, %f, %f, %f, %f\n", jColumn.first.x(), jColumn.first.y(), jColumn.first.z(),
                //                                            jColumn.second.x(), jColumn.second.y(), jColumn.second.z());
                
                J.prepend(jColumn);
                
                if ( joint->isBodyPartRoot() )
                    break;
            } //else printf("cast failed unexpectedly\n");
        } //else printf("found a non-joint\n");
        
        if (!thisNode->parent())
            break;
        else
            thisNode = thisNode->parent();
    }
    
    return J;
}


int KinTreeNode::getNumPrimitives()
{
	int result = primitives.size() + fieldPrimitives.size();
	QVector<KinTreeNode*>::iterator i;
    for ( i=children.begin(); i!=children.end(); ++i ) {
        result += (*i)->getNumPrimitives();
    }
	return result;
}

/*int KinTreeNode::getNumNodes()
{
	int result = 0;
	QVector<KinTreeNode*>::iterator i;
    for ( i=children.begin(); i!=children.end(); ++i ) {
        result += (*i)->getNumNodes();
    }
	return ++result;
}*/