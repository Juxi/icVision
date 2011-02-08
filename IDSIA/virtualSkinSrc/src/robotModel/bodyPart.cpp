#include <iostream>
#include "robot.h"
#include "robotmodelexception.h"

using namespace std;

BodyPart::BodyPart( Robot* robot, BodyPart* parentPart ) : parentPart(parentPart)
{
	if ( !robot ) { throw RobotModelException("The BodyPart constructor requires a pointer to a valid Robot."); }
    robot->appendBodyPart(this);
}
BodyPart::~BodyPart()
{
}
bool BodyPart::setEncPos( const QVector<qreal>& x )
{
    if ( x.size() > size() ) {
        cout << "WARNING BodyPart " << name().toStdString() << endl
             << "Position vector size > " << size() << ". Some values will be ignored." << endl;
        return 0;
    } else if ( x.size() < size() ) {
        cout << "WARNING BodyPart " << name().toStdString() << endl
             << "Position vector size < " << size() << ". Some motor positions will not be set." << endl;
    }
    QVector<Motor*>::iterator i;
    QVector<qreal>::const_iterator j;
    for ( i=begin(),j=x.begin(); i!=end() && j!=x.end(); ++i,++j ) {
        (*i)->setEncPos(*j);
    }
    return 1;
}
bool BodyPart::verify()
{
	bool result = true;
	QVector<Motor*>::iterator i;
    for ( i=begin(); i!=end(); ++i ) {
        if ( !(*i) ) result = false;
	}
	return result;
}
