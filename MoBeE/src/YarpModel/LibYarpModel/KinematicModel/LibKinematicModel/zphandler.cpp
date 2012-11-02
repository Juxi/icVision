#include "zphandler.h"
#include "model.h"
#include "robot.h"
#include "joint_revolute.h"
#include "joint_prismatic.h"
#include "cylinder.h"
#include "sphere.h"
#include "box.h"
//#include "displmatrix.h"
#include "modelexception.h"
#include "marker.h"

using namespace std;
using namespace KinematicModel;

ZPHandler::ZPHandler( Model* _model, Robot *robot) : model(_model), robot(robot)
{
	bodyPart = NULL;
	motor = NULL;
	node = NULL;
	noReflexRoot = NULL;
    constraintList = NULL;
	
	metKinTreeTag = false;
	markerCounter = 1;
}

bool ZPHandler::startElement( const QString & /* namespaceURI */,
                                   const QString & /* localName */,
                                   const QString &qName,
                                   const QXmlAttributes &attributes )
{
	/*****************************************************************************************
    *** READ THE DOCUMENT HEADER AND MAKE SURE IT IS ZERO-POSITION KINEMATIC TREE XML FILE ***
	*****************************************************************************************/
    if (!metKinTreeTag) {
		if ( QString::compare(qName,"ZeroPositionKinematicTree",caseSensitivity) != 0 ) {
			errorStr = "The file is not a 'ZeroPositionKinematicTree' file.";
			return false;
		} else {
			QString version = attributes.value("version");
			if (!version.isEmpty() && version != "1.0") {
				errorStr = "The file is not  a 'ZeroPositionKinematicTree' version 1.0 file.";
				return false;
			}
			metKinTreeTag = true;
			robot->setName( attributes.value("robotName") );
		}
    }
   
	/******************************************************************
	*** HANDLE BODY PARTS ... i.e. torso, head, left and right arms ***
	******************************************************************/
    else if ( QString::compare(qName,"bodyPart",caseSensitivity) == 0 ) {
        // check if the branch with this name already exists
        BodyPart* existingBodyPart;
        if ( !attributes.value("name").isEmpty() && (existingBodyPart = robot->getPartByName(attributes.value("name"))) ) {
            bodyPart = existingBodyPart; // if the branch by this name already exists, use the prexisting one
        } else {
            // make a new one
            if ( !(bodyPart = createChildPart()) ) return 0;
            if ( !attributes.value("name").isEmpty() ) bodyPart->setName( attributes.value("name") );
        }
    }
	
	/******************************************************************
	 *** HANDLE CONSTRAINTS ***
	 ******************************************************************/
	else if ( QString::compare(qName,"constraintList",caseSensitivity) == 0 ) {
        if ( !bodyPart ) printf("WARNING: ignoring <constraintList/> outside of <bodypart></>\n");
		else {
            constraintList = new QVector<LinearConstraint>();
        }
    }
    else if ( QString::compare(qName,"constraint",caseSensitivity) == 0 ) {
		//if ( !bodyPart ) printf("WARNING: ignoring <constraint/> outside of <bodypart></>\n");
        if ( !constraintList ) printf("WARNING: ignoring <constraint> outside of <constraintList></>\n");
		else {
			bool ok;
			double b = attributes.value("b").toDouble(&ok);
			QStringList a = attributes.value("a").split(" ",QString::SkipEmptyParts);
			QStringList q = attributes.value("q").split(" ",QString::SkipEmptyParts);
			bool negate = false;
            if ( attributes.value("negate") == "true" ) {
                negate = true;
                //printf("negated something\n");
            }
            
			if ( ok && a.size() > 0 && a.size() == q.size() )
			{
				//bodyPart->addConstraint( a, q, b );
                LinearConstraint c(bodyPart,a,q,b,negate);
                constraintList->append(c);
			}
			else { printf("WARNING: skipping badly formed <constraint/>\n"); }
		}
	}
	/***********************************************************************************
	*** HANDLE MOTORS ... i.e. the interface through which the robot should be moved ***
	***********************************************************************************/
    else if (QString::compare(qName,"motor",caseSensitivity) == 0) {
        Motor* existingMotor;
        if ( !attributes.value("name").isEmpty() && (existingMotor = robot->getMotorByName(attributes.value("name"))) ) {
            motor = existingMotor; /* if the motor by this name already exists, use the prexisting one 
								      and disregard any parameters set in this (second) definition */
        } else {
            // make a new one
			int motorIdx = -1; // negative motor indices are ignored
			if ( !attributes.value("idx").isEmpty() ) {
				bool ok = false;
				motorIdx = attributes.value("idx").toInt(&ok);
				if ( !ok || motorIdx < 0 ) printf("WARNING: Encountered Invalid Motor idx, which will be ignored. Configuration errors may occurr!\n");
			}
			
			if ( !(motor = createChildMotor( motorIdx )) ) return 0;
            if ( !attributes.value("name").isEmpty() ) motor->setName( attributes.value("name") );
            if ( !attributes.value("maxPos").isEmpty() ) motor->setMax( attributes.value("maxPos").toDouble() );
            if ( !attributes.value("minPos").isEmpty() ) motor->setMin( attributes.value("minPos").toDouble() );
			
			qreal homePos = 0;
			if ( !attributes.value("home").isEmpty() ) homePos = attributes.value("home").toDouble();
			motor->setHomePos( homePos );
        }
    }

	/*******************************************************************
	*** HANDLE JOINTS AND LINKS ... i.e. the kinematics of the robot ***
	*******************************************************************/
    else if ( QString::compare(qName,"link",caseSensitivity) == 0 || QString::compare(qName,"joint",caseSensitivity) == 0 ) {
        QVector3D axis = QVector3D( attributes.value("x").toDouble(),
                                    attributes.value("y").toDouble(),
                                    attributes.value("z").toDouble() );
        qreal radius = attributes.value("radius").toDouble(),
			  height = attributes.value("length").toDouble();
		
		if ( qFuzzyIsNull(height) ) { height = axis.length(); }
		
		axis = height * axis.normalized();

        if ( QString::compare(qName,"joint",caseSensitivity) == 0 ) {
            if ( !(node = createChildJoint(attributes.value("type"))) ) return 0;
            if ( !attributes.value("minPos").isEmpty() ) static_cast<Joint*>(node)->setMin( M_PI/180*attributes.value("minPos").toDouble() );
            if ( !attributes.value("maxPos").isEmpty() ) static_cast<Joint*>(node)->setMax( M_PI/180*attributes.value("maxPos").toDouble() );
        }
        else if ( QString::compare(qName,"link",caseSensitivity) == 0 ) {
            if ( !(node = createChildLink()) ) return 0;
        }

		try {
			node->setNodeAxis(axis);
			if ( radius > 0 )
			{
				PrimitiveObject* cylinder = new KinematicModel::Cylinder( radius, height );
				cylinder->setSpecialEulerOrientation(axis);
                
                PrimitiveObject* field = new KinematicModel::Cylinder( radius/2, height/2 );
				field->setSpecialEulerOrientation(axis);
				
				if (node->getNodeType() == KinTreeNode::LINK )
                {
                    cylinder->translate( axis/2 );
                    field->translate(axis/2);
                }
				else if ( node->getNodeType() != KinTreeNode::RJOINT &&  node->getNodeType() != KinTreeNode::PJOINT ) { 
					errorStr = "Encountered KinTreeNode of unknown type";
					return 0;
				}
				
				//node->appendPrimitive(cylinder);
                
                if (robot->hasField())
                    node->appendField(field);
			}
        }
		catch (std::exception& e)
		{
			errorStr = e.what();
			return 0;
		}
		
		if ( attributes.value("noSelfCollision") == "true" && !noReflexRoot )
			noReflexRoot = node;
			//node->setReflexSubtree(false);
    }

	/*******************************************************************************
	*** HANDLE EXTRA GEOMETRIES ... i.e. robot parts made from composite objects ***
	*******************************************************************************/
    else if ( QString::compare(qName,"sphere",caseSensitivity) == 0 || 
			  QString::compare(qName,"cylinder",caseSensitivity) == 0 || 
			  QString::compare(qName,"box",caseSensitivity) == 0)
	{
		if (!node)
		{
            errorStr = "Encountered <sphere/>, <cylinder/>, or <box/> outside of <link> and <joint>.";
            return 0;
        }
		
		/*
		 *	MAKE A GEOMETRY
		 */
		PrimitiveObject* primitive;
		try
		{
			if ( QString::compare(qName,"sphere",caseSensitivity) == 0 ) {
				qreal radius = attributes.value("radius").toDouble();
				primitive = new Sphere( radius );
			}
			else if ( QString::compare(qName,"cylinder",caseSensitivity) == 0 ) {
				qreal radius = attributes.value("radius").toDouble();
				qreal height = attributes.value("height").toDouble();
				primitive = new Cylinder( radius, height );
			}
			else if ( QString::compare(qName,"box",caseSensitivity) == 0 ) {
				QVector3D size = QVector3D(attributes.value("width").toDouble(),
										   attributes.value("height").toDouble(),
										   attributes.value("depth").toDouble());
				primitive = new Box( size );
			}
        }
		catch (std::exception& e)
		{ 
			errorStr = e.what();
			return 0;
		}
		
		/*
		 *	SET ITS ROTATION
		 */		
		QVector3D heightAxis = QVector3D( attributes.value("hx").toDouble(),
										  attributes.value("hy").toDouble(),
										  attributes.value("hz").toDouble() );
		qreal rotAngle = attributes.value("angle").toDouble()*M_PI/180;
		
		if ( qFuzzyIsNull( heightAxis.length() ) ) { heightAxis.setY(1); }
		
		//primitive->setCartesianOrientation(QVector3D(1,2,3));
		
		//primitive->specialEulerRotate(heightAxis,rotAngle);
		
		primitive->setSpecialEulerOrientation( heightAxis, rotAngle );
		
		/*
		 *	SET ITS POSITION
		 */
		QVector3D position = QVector3D( attributes.value("px").toDouble(),
									   attributes.value("py").toDouble(),
									   attributes.value("pz").toDouble() );
		primitive->translate(position);
		//primitive->setOpaque();
        
		//node->appendPrimitive(primitive);
    }

	/*******************************************************************************
	*** HANDLE MARKERS                                                           ***
	*******************************************************************************/
	else if ( QString::compare(qName,"marker",caseSensitivity) == 0)
	{
		if (!node)
		{
			errorStr = "Encountered <marker/>, outside of <link> and <joint>.";
			return false;
		}

		// obtain information
		QString name = attributes.value("name");
		if (name == "")
		{
			name = "marker" + QString(markerCounter);
			markerCounter++;
		}
        
        int tracer = 5;
        if (!attributes.value("tracer").isEmpty())
            tracer = attributes.value("tracer").toInt();
            
		// create the marker, attach it to the node, and make it known to the robot
		Marker* marker = new Marker(node, name);
		marker->createTracer( model->GHOST(), tracer, 0.008, Qt::yellow);
		robot->markers.push_back(marker);
        robot->countCompositeObject();
        bodyPart->markers.push_back(marker);
	}

    else { printf("WARNING: Encountered unknown tag: %s ...skipping it.\n",qName.toStdString().c_str()); }

    currentText.clear();
    return true;
}


bool ZPHandler::endElement(const QString & /* namespaceURI */, const QString & /* localName */, const QString &qName)
{
    if ( qName == "bodyPart") {
		if ( !bodyPart->verify() ) {
			errorStr = "Branch contains null pointers. Check index assignments. When assigning manually, every index 0..n must be used.";
			return 0;
		}
        bodyPart = bodyPart->parent();
    }
    else if ( qName == "constraintList" ) {
        if ( !bodyPart ) printf("WARNING: ignoring </constraintList> outside of <bodypart></>. Constraints will not be appended to body part.\n");
        else {
            bodyPart->appendConstraints(*constraintList);
        }
        delete constraintList;
        constraintList = NULL;
    }
    else if ( qName == "motor" ) {
        motor = motor->parent();
    }
    else if ( qName == "link" || qName == "joint" )
	{
		robot->getModel()->appendObject(node);
		if ( noReflexRoot && node == noReflexRoot )
		{
			node->removeReflexFromSubTree();
			noReflexRoot = NULL;
		}
        
        robot->countCompositeObject();
		node = node->parent();
    }
    //else if ( qName == "object" ) {
    //    robot->world.append(object);
    //    object = object->parent();
    //}

    return true;
}


BodyPart* ZPHandler::createChildPart()
{
	try
	{
		BodyPart* childPart = new BodyPart( robot, bodyPart );
		if (!childPart) errorStr = "Failed to allocate bodyPart.";
		return childPart;
	}
	catch (std::exception& e)
	{ 
		errorStr = e.what();
		return 0;
	}
}

Motor* ZPHandler::createChildMotor( int motorIdx )
{
    if ( !bodyPart ) {
        errorStr = "Ecnountered <motor> outside of <bodyPart>!!";
        return 0;
    }

	try
	{
		Motor* childMotor = new Motor( robot, motor );
		if (childMotor) {
            if (bodyPart->size()==0)
                childMotor->bodyPartRoot = true;
			if ( bodyPart->size() <= motorIdx )
				bodyPart->resize(motorIdx+1);
			if ( motorIdx >= 0 ) {
				if ( bodyPart->at(motorIdx) ) 
				{
					printf("WARNING: Replacing motor '%d' in body part '%s'. Errors may occurr.\n", motorIdx, bodyPart->name().toStdString().c_str() );
				}
				bodyPart->replace(motorIdx,childMotor);
			} else {
				bodyPart->append(childMotor);
			}
		} else errorStr = "Failed to allocate motor.";
		return childMotor;
	}
	catch (std::exception& e)
	{ 
		errorStr = e.what();
		return 0;
	}
}

KinTreeNode* ZPHandler::createChildLink()
{
	try
	{
		KinTreeNode* link = new Link( robot, bodyPart->index(), node );
		if (!link) errorStr = "Failed to allocate link.";
		return link;
	}
	catch (std::exception& e)
	{ 
		errorStr = e.what();
		return 0;
	}
    
}

KinTreeNode* ZPHandler::createChildJoint( const QString& type )
{
    if ( !motor )
	{
        errorStr = "Ecnountered <joint> outside of <motor>!!\n";
        return 0;
    }
	
	try
	{
		KinTreeNode* joint;
		if ( type == "revolute" || type == "" ) { joint = new RevoluteJoint( robot, bodyPart->index(), node, motor); }
		else if ( type == "prismatic" )			{ errorStr = "Prismatic joints are not yet supported\n"; }
		else									{ errorStr = "Tried to create <joint> of unknown type!! Known types are 'revolute' and 'prismatic'\n"; }
		// TODO: add prismatic joints
		if (!joint) errorStr = "Failed to allocate joint.\n";
		return joint;
	}
	catch (std::exception& e)
	{ 
		errorStr = e.what();
		return 0;
	}

}

bool ZPHandler::characters(const QString &str)
{
    currentText += str;
    return true;
}
bool ZPHandler::fatalError(const QXmlParseException &exception)
{
	printf("XML PARSE FATAL ERROR: line %d column %d\n", exception.lineNumber(), exception.columnNumber());
	printf("%s\n", exception.message().toStdString().c_str());
    return false;
}

QString ZPHandler::errorString() const
{
    return errorStr;
}
