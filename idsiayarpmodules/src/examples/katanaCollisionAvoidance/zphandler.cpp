#include <iostream>
#include "zphandler.h"
#include "robot.h"
#include "joint.h"
#include "joint_revolute.h"
#include "joint_prismatic.h"
#include "cylinder.h"
#include "sphere.h"
#include "box.h"
#include "displmatrix.h"

using namespace std;

ZPHandler::ZPHandler(Robot *robot) : robot(robot)
{
    branch = 0;
    motor = 0;
    node = 0;
    object = 0;
    metKinTreeTag = false;
}

bool ZPHandler::startElement( const QString & /* namespaceURI */,
                                   const QString & /* localName */,
                                   const QString &qName,
                                   const QXmlAttributes &attributes )
{
    // READ THE DOCUMENT HEADER AND MAKE SURE IT IS ZERO-POSITION KINEMATIC TREE XML FILE
    if (!metKinTreeTag && qName != "ZeroPositionKinematicTree") {
        errorStr = "The file is not a 'Zero Position Kinematic Tree' file.";
        return false;
    }
    if (qName == "ZeroPositionKinematicTree") {
        QString version = attributes.value("version");
        if (!version.isEmpty() && version != "1.0") {
            errorStr = "The file is not  a 'Zero Position Kinematic Tree' version 1.0 file.";
            return false;
        }
        metKinTreeTag = true;
        robot->setName( attributes.value("robotName") );
    }

    else if (qName == "branch") {
        //cout << "Encountered <branch>" << endl;
        // check if the branch with this name already exists
        Branch* existingBranch;
        if ( !attributes.value("name").isEmpty() && (existingBranch = robot->getBranchByName(attributes.value("name"))) ) {
            branch = existingBranch; // if the branch by this name already exists, use the prexisting one
        } else {
            // make a new one
            if ( !(branch = createChildBranch()) ) return 0;
            if ( !attributes.value("name").isEmpty() ) branch->setName( attributes.value("name") );
        }
    }

    else if (qName == "motor") {
        //cout << "  Encountered <motor>" << endl;
        Motor* existingMotor;
        if ( !attributes.value("name").isEmpty() && (existingMotor = robot->getMotorByName(attributes.value("name"))) ) {
            motor = existingMotor; // if the motor by this name already exists, use the prexisting one
        } else {
            // make a new one
            if ( !(motor = createChildMotor()) ) return 0;
            if ( !attributes.value("name").isEmpty() ) motor->setName(attributes.value("name"));
            if ( !attributes.value("minPos").isEmpty() ) motor->setMin( attributes.value("minPos").toDouble() );
            if ( !attributes.value("maxPos").isEmpty() ) motor->setMax( attributes.value("maxPos").toDouble() );
        }
    }

    else if (qName == "link" || qName == "joint") {
        QVector3D axis = QVector3D( attributes.value("x").toDouble(),
                                    attributes.value("y").toDouble(),
                                    attributes.value("z").toDouble() );
        qreal radius = attributes.value("radius").toDouble(),
              length = axis.length();

        if ( radius < 0 ) {
            // could cout a warning here
            errorStr = "If the property 'radius' is specified, it must be >= 0.";
            return 0;
        }

        if ( !attributes.value("length").isEmpty() ) {
            if ( attributes.value("length").toDouble() <= 0 ) {
                // could cout a warning here
                errorStr = "If the property 'length' is specified, it must be > 0";
                return 0;
            }
            length = attributes.value("length").toDouble();
            axis = length * axis.normalized();
        }

        int lod = attributes.value("detail").toInt();
        if ( lod < 0 ) {
            errorStr = "The property 'detail' must be >= 0.";
            return 0;
        }

        if ( qName == "joint" ) {
			Joint* joint;
            if ( !(joint = createChildJoint(attributes.value("type"))) ) return 0;
			if ( !attributes.value("minPos").isEmpty() ) joint->setMin( attributes.value("minPos").toDouble()*M_PI/180 );
            if ( !attributes.value("maxPos").isEmpty() ) joint->setMax( attributes.value("maxPos").toDouble()*M_PI/180 );
			node = joint;
        }
        else if ( qName == "link" ) {
            if ( !(node = createChildLink()) ) return 0;
        }

        if ( !attributes.value("name").isEmpty() ) node->setName(attributes.value("name"));
        if ( !node->setAxis(axis) ) {
            errorStr = "Axis must be non-zero.";
            return 0;
        }
        if ( radius > 0 && axis.length() > 0 ) {
            if (node->type() == KinTreeNode::LINK ) node->appendToObject( new Cylinder( radius, axis.length(), axis/2, axis, lod) );
            else if (node->type() == KinTreeNode::RJOINT ) node->appendToObject( new Cylinder( radius, axis.length(), QVector3D(), axis, lod) );
            else { errorStr = "Encountered KinTreeNode of unknown type"; return 0; }
        }
    }

    else if (qName == "object") {
        object = new Object(object);
    }

    else if ( qName == "sphere" || qName == "cylinder" || qName == "box" ) {
        PrimitiveObject* primitive;

        QVector3D position;
        if (node) position = node->axis()/2;
        if ( !attributes.value("px").isEmpty() ||
             !attributes.value("py").isEmpty() ||
             !attributes.value("pz").isEmpty()    ){
            position = QVector3D( attributes.value("px").toDouble(),
                                  attributes.value("py").toDouble(),
                                  attributes.value("pz").toDouble() );
        }

        QVector3D axis;
        if (node) axis = node->axis();
        if ( !attributes.value("vx").isEmpty() ||
             !attributes.value("vy").isEmpty() ||
             !attributes.value("vz").isEmpty()    ){
            axis = QVector3D( attributes.value("vx").toDouble(),
                              attributes.value("vy").toDouble(),
                              attributes.value("vz").toDouble() );
        }
        if ( qFuzzyCompare(axis.length(),0) ) {
            errorStr = "The requested geomotry requires an axis. use vx='', vy='', vz=''";
            return 0;
        }

        //////////////////////////////////////////////////////////////////////
        if (qName == "sphere") {
            qreal radius = attributes.value("radius").toDouble();
            if ( radius <= 0 ) {
                errorStr = "The property 'radius' must be > 0.";
                return 0;
            }

            int lod = attributes.value("detail").toInt();
            if ( lod < 0 ) {
                errorStr = "The property 'detail' must be >= 0.";
                return 0;
            }

            primitive = new Sphere( radius, position, lod);
        }
        //////////////////////////////////////////////////////////////////////
        else if (qName == "cylinder") {
            qreal radius = attributes.value("radius").toDouble();
            if ( radius <= 0 ) {
                errorStr = "The property 'radius' must be > 0.";
                return 0;
            }

            qreal height = attributes.value("height").toDouble();
            if ( height <= 0 ) {
                errorStr = "The property 'height' must be > 0.";
                return 0;
            }

            int lod = attributes.value("detail").toInt();
            if ( lod < 0 ) {
                errorStr = "The property 'detail' must be >= 0.";
                return 0;
            }

            primitive = new Cylinder( radius, height, position, axis, lod);
        }
        //////////////////////////////////////////////////////////////////////
        else if (qName == "box") {
            qreal angle = attributes.value("angle").toDouble();
            qreal x = attributes.value("x").toDouble(); if ( x <= 0 ) {errorStr = "x must be > 0."; return 0;}
            qreal y = attributes.value("y").toDouble(); if ( y <= 0 ) {y = node->axis().length();}
            qreal z = attributes.value("z").toDouble(); if ( z <= 0 ) {errorStr = "z must be > 0."; return 0;}

            if ( x == 0 || y == 0 || z == 0 ) { errorStr = "x or y or z must be > 0"; return 0;}

            primitive = new Box( x, y, z, position, axis, angle );
        }
        //////////////////////////////////////////////////////////////////////
        if (object)
            object->append(primitive);
        else if (node)
            node->appendToObject(primitive);
        else {
            errorStr = "Encountered <sphere/>, <cylinder/>, or <box/> outside of <object>, <link>, and <joint>.";
            return 0;
        }
    }

    else { cout << "WARNING: Encountered unknown tag '" << qName.toStdString() << "'" << endl; /*return false;*/ }

    currentText.clear();
    return true;
}


bool ZPHandler::endElement(const QString & /* namespaceURI */, const QString & /* localName */, const QString &qName)
{
    if ( qName == "branch") {
        branch = branch->parent();
    }
    else if ( qName == "motor" ) {
        motor->setEncPos(0);
        motor = motor->parent();
    }
    else if ( qName == "link" || qName == "joint" ) {
        node->emitFinished();
        node = node->parent();
    }
    else if ( qName == "object" ) {
        robot->world.append(object);
        object = object->parent();
    }

    return true;
}


Branch* ZPHandler::createChildBranch()
{
    Branch* childBranch = 0;
    if (branch) childBranch = new Branch( robot, branch );
    else        childBranch = new Branch( robot );

    if (!childBranch) errorStr = "Failed to allocate branch.";
    return childBranch;
}

Motor* ZPHandler::createChildMotor()
{
    if ( !branch ) {
        errorStr = "Ecnountered <motor> outside of <branch>!!";
        return 0;
    }

    Motor* childMotor = 0;
    if (motor) childMotor = new Motor( robot, motor );
    else       childMotor = new Motor( robot );

    if (childMotor) branch->append(childMotor);
    else errorStr = "Failed to allocate motor.";
    return childMotor;
}

KinTreeNode* ZPHandler::createChildLink()
{
    KinTreeNode* link = 0;
    if (node) link = new Link(node);
    else      link = new Link(robot);

    if (!link) errorStr = "Failed to allocate link.";
    return link;
}

Joint* ZPHandler::createChildJoint( const QString& type )
{
    if ( !motor ) {
        errorStr = "Ecnountered <joint> outside of <motor>!!";
        return 0;
    }
    if ( type != "revolute" && type != "prismatic" && type != "" ) {
        errorStr = "Tried to create <joint> of unknown type!! Known types are 'revolute' and 'prismatic'";
        return 0;
    }
    if ( type == "prismatic" ) {
        errorStr = "Prismatic joints are not yet supported";
        return 0;
    }

    Joint* joint = 0;
    if (node) {
        if ( type == "revolute" || type == "" )  joint = new RevoluteJoint(node, motor);
        else if ( type == "prismatic" )          joint = new PrismaticJoint(node, motor);
    } else {
        if ( type == "revolute" || type == "" )  joint = new RevoluteJoint(robot, motor);
        else if ( type == "prismatic" )          joint = new PrismaticJoint(robot, motor);
    }

    if (!joint) errorStr = "Failed to allocate joint.";
    return joint;
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
