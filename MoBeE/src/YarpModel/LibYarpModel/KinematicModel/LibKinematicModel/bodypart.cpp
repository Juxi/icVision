#include "robot.h"
#include "modelexception.h"

using namespace KinematicModel;

BodyPart::BodyPart( Robot* robot, BodyPart* parentPart ) : idx(0), parentPart(parentPart)
{
	if ( !robot ) { throw KinematicModelException("The BodyPart constructor requires a pointer to a valid Robot."); }
    robot->appendBodyPart(this);
}
BodyPart::~BodyPart()
{
}

//void BodyPart::addConstraint( QStringList _a, QStringList _q, qreal _b )
//{
//	LinearConstraint c(this,_a,_q,_b);
//	constraints.append(c);
//	//printf("Added constraint %d to %s\n", constraints.size(), partName.toStdString().c_str());
//}

bool BodyPart::evaluateConstraints()
{
	int count = 0;
	QVector< QVector<LinearConstraint> >::iterator i;
    QVector<LinearConstraint>::iterator j;
	for ( i=constraints.begin(); i!=constraints.end(); ++i )
    {
        bool inner = false;
        QVector< QVector<qreal> > springs;
        
        // disjunctive list A OR B OR C... etc
        for ( j=i->begin(); j!=i->end(); ++j )
        {
            QVector<qreal> thisSpring;
            if (j->evaluate(thisSpring))
            {
                inner = true;
                //printf("pass\n");
                break;
            }
            springs.append(thisSpring);
        }
        
		if ( !inner ) {
            //printf("Body Part %d: Linear Constraint(s) failed... starting reflex\n", idx);
			return false;
        }
        
		//	printf( "  constraint %d: pass\n", count++ );
		//else {
			//printf( "  constraint %d: fail\n", count++ );
		//	result = false;
		//}
	}
	return true;
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

void BodyPart::publishState()
{
    QVector<qreal> t,s;
    QVector<Motor*>::iterator i;
    for ( i=begin(); i!=end(); ++i )
	{
        t.append((*i)->getTorque());
        s.append((*i)->getSpring());
	}
    emit repulsiveForce(t);
    emit constraintSpring(s);
}
