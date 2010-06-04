#include <iostream>
#include "zphandler.h"
#include "robot.h"
#include "joint_revolute.h"
#include "joint_prismatic.h"
#include "cylinder.h"
#include "displmatrix.h"

using namespace std;

ZPHandler::ZPHandler(Robot *robot) : robot(robot)
{
    branch = 0;
    motor = 0;
    link = 0;
    //joint = 0; // same as the link - just need this for the datatype
    metKinTreeTag = false;
}

bool ZPHandler::startElement( const QString & /* namespaceURI */,
                                   const QString & /* localName */,
                                   const QString &qName,
                                   const QXmlAttributes &attributes )
{
    // READ THE DOCUMENT HEADER AND MAKE SURE IT IS ZERO-POSITION KINEMATIC TREE XML FILE
    if (!metKinTreeTag && qName != "ZeroPositionKinematicTree") {
        cout << "The file is not a 'Zero Position Kinematic Tree' file." << endl;
        return false;
    }
    if (qName == "ZeroPositionKinematicTree") {
        QString version = attributes.value("version");
        if (!version.isEmpty() && version != "1.0") {
            cout << "The file is not  a 'Zero Position Kinematic Tree' version 1.0 file.";
            return false;
        }
        metKinTreeTag = true;
        robot->robotName = attributes.value("robotName");
    }

    else if (qName == "branch") {
        //cout << "Encountered <branch>" << endl;
        // check if the branch with this name already exists
        Branch* existingBranch;
        if ( !attributes.value("name").isEmpty() && (existingBranch = robot->getBranchByName(attributes.value("name"))) ) {
            branch = existingBranch; // if the branch by this name already exists, use the prexisting one
        } else {
            // make a new one
            if ( !(branch = createChildBranch()) ) { cout << "Failed to create child branch." << endl; return 0; }
            if ( !attributes.value("name").isEmpty() ) { branch->setName( attributes.value("name") ); }
        }
    }

    else if (qName == "motor") {
        //cout << "  Encountered <motor>" << endl;
        Motor* existingMotor;
        if ( !attributes.value("name").isEmpty() && (existingMotor = robot->getMotorByName(attributes.value("name"))) ) {
            motor = existingMotor; // if the motor by this name already exists, use the prexisting one
        } else {
            // make a new one
            if ( !(motor = createChildMotor()) ) { cout << "Failed to create child motor." << endl; return 0; }
            if ( !attributes.value("name").isEmpty() ) { motor->motorName = attributes.value("name"); }
            if ( !attributes.value("minPos").isEmpty() ) { motor->motorLimits.setMin( attributes.value("minPos").toDouble() ); }
            if ( !attributes.value("maxPos").isEmpty() ) { motor->motorLimits.setMax( attributes.value("maxPos").toDouble() ); }
        }
    }

    else if (qName == "link") {
        QVector3D bodyVector = QVector3D( attributes.value("x").toDouble(),
                                          attributes.value("y").toDouble(),
                                          attributes.value("z").toDouble() );
        qreal radius = attributes.value("radius").toDouble(),
              length = attributes.value("length").toDouble();
        if ( !length ) length = bodyVector.length();

        link = createChildLink();
        link->linkName = attributes.value("name");
        link->bodyVector = bodyVector;
        link->M = DisplMatrix( QVector3D(0,0,0), bodyVector, 0, length );
        link->physObjectList.append( new Cylinder( radius, length, length/2*bodyVector.normalized(), bodyVector) );
    }
    else if (qName == "joint") {
        QVector3D jointAxis = QVector3D( attributes.value("x").toDouble(),
                                         attributes.value("y").toDouble(),
                                         attributes.value("z").toDouble() );
        qreal radius = attributes.value("radius").toDouble(),
              length = attributes.value("length").toDouble();
        if ( !length ) length = jointAxis.length();

        if ( jointAxis.length() == 0 ) {
            cout << "XML PARSE ERROR: <joint> must have a non-zero joint axis vector!! You must specify at least one of x='' y='' z=''." << endl;
            return 0;
        }

        createJoint( attributes.value("type") );
        link->joint->axis = jointAxis;
        if ( !attributes.value("minPosition").isEmpty() ) { link->joint->limits.setMin( attributes.value("minPosition").toDouble() ); }
        if ( !attributes.value("maxPosition").isEmpty() ) { link->joint->limits.setMax( attributes.value("maxPosition").toDouble() ); }
        link->physObjectList.append( new Cylinder( radius, length, link->bodyVector, link->joint->axis) );
    }
    //else if (qName == "geom") {}

    else { cout << "WARNING: Encountered unknown tag '" << qName.toStdString() << "'" << endl; /*return false;*/ }
    currentText.clear();
    return true;
}


bool ZPHandler::endElement(const QString & /* namespaceURI */, const QString & /* localName */, const QString &qName)
{
    if ( qName == "branch") {
        //cout << "Encountered </branch>" << endl;
        branch = branch->parent();
    }
    else if ( qName == "motor" ) {
        //cout << "  Encountered </motor>" << endl;
        motor = motor->parent();
    }
    else if ( qName == "link" ) {
        //cout << "    Encountered </link>" << endl;
        link->emitFinished();
        link = link->parent();
    }

    return true;
}


Branch* ZPHandler::createChildBranch()
{
    Branch* childBranch;
    if (branch) {
        if ( !(childBranch = new Branch( robot, branch )) ) {
            cout << "Failed to allocate child branch." << endl;
            return 0;
        }
    } else {
        if ( !(childBranch = new Branch( robot )) ) {
            cout << "Failed to allocate child branch." << endl;
            return 0;
        }
    }
    return childBranch;
}

Motor* ZPHandler::createChildMotor()
{
    if ( !branch ) {
        cout << "XML PARSE ERROR: Ecnountered <motor> outside of <branch>!!" << endl;
        return 0;
    }
    Motor* childMotor;
    if (motor) {
        if ( !(childMotor = new Motor( robot, motor )) ) {
            cout << "Failed to allocate child motor." << endl;
            return 0;
        }
    } else {
        if ( !(childMotor = new Motor( robot )) ) {
            cout << "Failed to allocate child motor." << endl;
            return 0;
        }
    }
    branch->append(childMotor);
    return childMotor;
}

Link* ZPHandler::createChildLink()
{
    Link* child;
    if (link) {
        if ( !(child = new Link(link)) ) {
            cout << "Failed to allocate child link." << endl;
            return 0;
        }
    } else {
        if ( !(child = new Link(robot)) ) {
            cout << "Failed to allocate child link." << endl;
            return 0;
        }
    }
    return child;
}

bool ZPHandler::createJoint( const QString& type )
{
    if ( !link ) {
        cout << "XML PARSE ERROR: Ecnountered <joint> outside of <link>!!" << endl;
        return 0;
    }
    if ( !motor ) {
        cout << "XML PARSE ERROR: Ecnountered <joint> outside of <motor>!!" << endl;
        return 0;
    }
    if ( link->getJoint() ) {
        cout << "XML PARSE ERROR: More than one <joint> within <link>!!" << endl;
        return 0;
    }
    if ( type != "revolute" && type != "reismatic" && type != "" ) {
        cout << "XML PARSE ERROR: Tried to create <joint> of unknown type!! Known types are 'revolute' and 'prismatic'" << endl;
        return 0;
    }

    Joint* joint;
    if ( type == "revolute" || type == "" ) {
        if ( !(joint = new RevoluteJoint(motor)) ) {
            cout << "Failed to allocate joint." << endl;
            return 0;
        }
    }
    else if ( type == "prismatic" ) {
        if ( !(joint = new PrismaticJoint(motor)) ) {
            cout << "Failed to allocate joint." << endl;
            return 0;
        }
    }
    link->setJoint(joint);
    return 1;
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
bool ZPHandler::error(const QXmlParseException &exception)
{
    cout << "XML PARSE ERROR: line " << exception.lineNumber() << " column " << exception.columnNumber() << endl
         << exception.message().toStdString() << endl;
    return false;
}
bool ZPHandler::warning(const QXmlParseException &exception)
{
    cout << "XML PARSE WARNING: line " << exception.lineNumber() << " column " << exception.columnNumber() << endl
         << exception.message().toStdString() << endl;
    return false;
}
QString ZPHandler::errorString() const
{
    return errorStr;
}
