#include <iostream>
#include "zphandler.h"
#include "robot.h"
#include "joint_revolute.h"
#include "joint_prismatic.h"
#include "cylinder.h"
#include "sphere.h"
#include "box.h"
#include "displmatrix.h"
#include "robotmodelexception.h"

using namespace std;

ZPHandler::ZPHandler(Robot *robot) : robot(robot)
{
    bodyPart = 0;
    motor = 0;
    node = 0;
    //object = 0;
    metKinTreeTag = false;
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
			cout << QString::compare(qName,"ZeroPositionKinematicTree",caseSensitivity) << endl;
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
			cout << "Robot Name: " << attributes.value("robotName").toStdString() << endl;
		}
    }
   
	/******************************************************************
	*** HANDLE BODY PARTS ... i.e. torso, head, left and right arms ***
	******************************************************************/
    else if ( QString::compare(qName,"bodyPart",caseSensitivity) == 0 ) {
        //cout << "Encountered <bodyPart>" << endl;
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

	/***********************************************************************************
	*** HANDLE MOTORS ... i.e. the interface through which the robot should be moved ***
	***********************************************************************************/
    else if (QString::compare(qName,"motor",caseSensitivity) == 0) {
        //cout << "  Encountered <motor>" << endl;
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
				if ( !ok || motorIdx < 0 ) cout << "WARNING: Encountered Invalid Motor idx, which will be ignored. Configuration errors may occurr!" << endl;
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
				PrimitiveObject* cylinder = node->newCylinder( radius, height );
				cylinder->specialRotate(axis);
				
				if (node->getNodeType() == KinTreeNode::LINK ) { cylinder->translate( axis/2 ); }
				else if ( node->getNodeType() != KinTreeNode::RJOINT &&  node->getNodeType() != KinTreeNode::PJOINT ) { 
					errorStr = "Encountered KinTreeNode of unknown type";
					return 0;
				}
			}
        }
		catch (RobotModelException e) { errorStr = e.what(); return 0;}
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
				primitive = node->newSphere(radius);
			}
			else if ( QString::compare(qName,"cylinder",caseSensitivity) == 0 ) {
				qreal radius = attributes.value("radius").toDouble();
				qreal height = attributes.value("height").toDouble();
				primitive = node->newCylinder( radius, height );
			}
			else if ( QString::compare(qName,"box",caseSensitivity) == 0 ) {
				QVector3D size = QVector3D(attributes.value("width").toDouble(),
										   attributes.value("height").toDouble(),
										   attributes.value("depth").toDouble());
				primitive = node->newBox( size );
			}
        }
		catch (RobotModelException e) { errorStr = e.what(); return 0;}
		
		/*
		 *	SET ITS ROTATION
		 */		
		QVector3D heightAxis = QVector3D( attributes.value("hx").toDouble(),
										  attributes.value("hy").toDouble(),
										  attributes.value("hz").toDouble() );
		qreal rotAngle = attributes.value("angle").toDouble()*M_PI/180;
		
		if ( qFuzzyIsNull( heightAxis.length() ) ) { heightAxis.setY(1); }
		
		primitive->specialRotate( heightAxis, rotAngle );
		
		/*
		 *	SET ITS POSITION
		 */
		QVector3D position = QVector3D( attributes.value("px").toDouble(),
									   attributes.value("py").toDouble(),
									   attributes.value("pz").toDouble() );
		primitive->translate(position);
    }

    else { cout << "WARNING: Encountered unknown tag...  skipping it. '" << qName.toStdString() << "'" << endl; /*return false;*/ }

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
    else if ( qName == "motor" ) {
        motor = motor->parent();
    }
    else if ( qName == "link" || qName == "joint" ) {
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
	catch (RobotModelException e) { errorStr = e.what(); return 0;}
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
			if ( bodyPart->size() <= motorIdx ) {
				bodyPart->resize(motorIdx+1);
			}
			if ( motorIdx >= 0 ) {
				if ( bodyPart->at(motorIdx) ) 
				{
					cout << "WARNING: Replacing motor " << motorIdx
					<< " in BodyPart: " << bodyPart->name().toStdString() 
					<< ". Errors may occurr..." << endl;
				}
				bodyPart->replace(motorIdx,childMotor);
			} else {
				bodyPart->append(childMotor);
			}
		} else errorStr = "Failed to allocate motor.";
		return childMotor;
	}
	catch (RobotModelException e) { errorStr = e.what(); return 0;}
}

KinTreeNode* ZPHandler::createChildLink()
{
	try
	{
		KinTreeNode* link = new Link(robot,node);
		if (!link) errorStr = "Failed to allocate link.";
		return link;
	}
	catch (RobotModelException e) { errorStr = e.what(); return 0;}
    
}

KinTreeNode* ZPHandler::createChildJoint( const QString& type )
{
    if ( !motor )
	{
        errorStr = "Ecnountered <joint> outside of <motor>!!";
        return 0;
    }
	
	try
	{
		KinTreeNode* joint;
		if ( type == "revolute" || type == "" ) { joint = new RevoluteJoint(robot, node, motor); }		
		else if ( type == "prismatic" )			{ errorStr = "Prismatic joints are not yet supported"; }
		else									{ errorStr = "Tried to create <joint> of unknown type!! Known types are 'revolute' and 'prismatic'"; }
		// TODO: add prismatic joints
		if (!joint) errorStr = "Failed to allocate joint.";
		return joint;
	}
	catch (RobotModelException e) { errorStr = e.what(); return 0;}

}

bool ZPHandler::characters(const QString &str)
{
    currentText += str;
    return true;
}
bool ZPHandler::fatalError(const QXmlParseException &exception)
{
    cout << "XML PARSE FATAL ERROR: line " << exception.lineNumber() << " column " << exception.columnNumber() << endl
         << exception.message().toStdString() << endl;
    return false;
}
/*bool ZPHandler::error(const QXmlParseException &exception)
{
    cout << "XML PARSE ERROR: line " << exception.lineNumber() << " column " << exception.columnNumber() << endl
         << exception.message().toStdString() << endl;
    return false;
}*/
/*bool ZPHandler::warning(const QXmlParseException &exception)
{
    cout << "XML PARSE WARNING: line " << exception.lineNumber() << " column " << exception.columnNumber() << endl
         << exception.message().toStdString() << endl;
    return false;
}*/
QString ZPHandler::errorString() const
{
    return errorStr;
}
