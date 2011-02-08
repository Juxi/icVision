#include "solid.h"
#include "kintreenode.h"
#include "primitiveobject.h"
#include "robot.h"
#include "robotmodelexception.h"

KinTreeNode::KinTreeNode( Robot* robot, KinTreeNode* parent, Type type ) :  parentRobot(robot),
																		    parentNode(parent),
																		    index(robot->nextLinkIdx()),
																		    nodeType(type)
{
	if ( !robot ) { throw RobotModelException("The KinTreeNode constructor requires a pointer to a valid Robot."); }
	
    objectName.setNum(index);
    objectName.prepend("BodyPart");
    
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
	
	//printf("\n Deleting KinTreeNode %d with DisplayList %d !!!", index, displayListIdx() );
}

void KinTreeNode::setNodeAxis( const QVector3D& vector )
{
    if ( vector.length() <= 0 ) { throw RobotModelException("Axis must have non zero magnitude (set x,y or z > 0)"); }
	nodeAxis = vector;
	setM();
}

void KinTreeNode::append( PrimitiveObject* anObject )
{ 
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
    //cout << "Called " << this->name().toStdString() << ".filterCollisionPairs()" << endl;
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
        if ( foundLink && foundJoint && getNodeType() != node->getNodeType() ) {
			//if ( (node->getNodeType() == LINK && getNodeType() == RJOINT) ||
			//	(node->getNodeType() == RJOINT) && getNodeType() == ) 
			//{ 
				return;
			//}
			// TODO: handle PJOINT
		}

        if ( getNodeType() == LINK && parent()->getNodeType() != LINK && !isEmpty() ) {
            foundLink = true;
        }
        else if ( getNodeType() != LINK && parent()->getNodeType() == LINK && !isEmpty() ) {
            foundJoint = true;
        }
    }

    // do not check collision between this object and the one in 'node'
    doNotCheckCollision(static_cast<CompositeObject*>(node));

    QVector<KinTreeNode*>::iterator i = 0;
    for ( i=children.begin(); i!=children.end(); ++i ) {
        (*i)->serialFilter(node,foundLink,foundJoint);
    }
}

void KinTreeNode::render()
{ 
	mutex.lock();
		CompositeObject::render();
		QVector<KinTreeNode*>::iterator j;
		for ( j=children.begin(); j!=children.end(); ++j ) {
			(*j)->render();
		}
	mutex.unlock();
}

void KinTreeNode::update( const QMatrix4x4& txfr )
{
	mutex.lock();
	
		// update the physical objects that constitute this link
		setT( txfr );

		// update this link's children
		QMatrix4x4 nextT = txfr * M;
		QVector<KinTreeNode*>::iterator j;
		for ( j=children.begin(); j!=children.end(); ++j ) {
			(*j)->update(nextT);
		}
	
	mutex.unlock();
}

void KinTreeNode::notColliding()
{
	mutex.lock();
		CompositeObject::notColliding();
		QVector<KinTreeNode*>::iterator j;
		for ( j=children.begin(); j!=children.end(); ++j ) {
			(*j)->notColliding();
		}
	mutex.unlock();
}

void KinTreeNode::print() const
{
    const qreal* m = M.constData();
    const qreal* t = T.constData();
    //Alex: QDataStream??
	std::cout << "   ---------------------------------------------------" << std::endl
              //<< "    Node " << index << ": " << nodeName.toStdString()   << std::endl
              << "   ---------------------------------------------------" << std::endl
              << "      Axis:     " << nodeAxis.x() << " " << nodeAxis.y() << " " << nodeAxis.z() << std::endl
              << "      M:        " << m[0] << " " << m[4] << " " << m[8]  << " " << m[12] << std::endl
              << "                " << m[1] << " " << m[5] << " " << m[9]  << " " << m[13] << std::endl
              << "                " << m[2] << " " << m[6] << " " << m[10] << " " << m[14] << std::endl
              << "                " << m[3] << " " << m[7] << " " << m[11] << " " << m[15] << std::endl
              << "      T:        " << t[0] << " " << t[4] << " " << t[8]  << " " << t[12] << std::endl
              << "                " << t[1] << " " << t[5] << " " << t[9]  << " " << t[13] << std::endl
              << "                " << t[2] << " " << t[6] << " " << t[10] << " " << t[14] << std::endl
              << "                " << t[3] << " " << t[7] << " " << t[11] << " " << t[15] << std::endl
              << "   ---------------------------------------------------" << std::endl;
}

void KinTreeNode::printAll()
{
    print();
    QVector<KinTreeNode*>::iterator i;
    for ( i=children.begin(); i!=children.end(); ++i ) {
        (*i)->printAll();
    }
}
