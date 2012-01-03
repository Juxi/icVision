#include "robot.h"
#include "modelexception.h"

using namespace KinematicModel;

BodyPart::BodyPart( Robot* robot, BodyPart* parentPart ) : parentPart(parentPart)
{
	if ( !robot ) { throw KinematicModelException("The BodyPart constructor requires a pointer to a valid Robot."); }
    robot->appendBodyPart(this);
}
BodyPart::~BodyPart()
{
}
bool BodyPart::setEncPos( const QVector<qreal>& x )
{
    if ( x.size() > size() )
	{
		printf("WARNING: BodyPart %s - position vector size > %d. Some values will be ignored.\n", name().toStdString().c_str(), size() );
    }
	else if ( x.size() < size() )
	{
		printf("WARNING: BodyPart %s - position vector size < %d. Some values will not be affected.\n", name().toStdString().c_str(), size() );
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
    for ( i=begin(); i!=end(); ++i )
	{
        if ( !(*i) ) { result = false; }
	}
	return result;
}
