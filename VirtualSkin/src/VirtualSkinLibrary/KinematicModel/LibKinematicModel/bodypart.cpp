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
	//int count = 1;
	//bool result = true;
	//printf("Evaluating constraints for %s, size: %d\n",partName.toStdString().c_str(),constraints.size());
	QVector< QVector<LinearConstraint> >::iterator i;
    QVector<LinearConstraint>::iterator j;
    printf("evaluating %d body part constraint lists\n", constraints.size());
	for ( i=constraints.begin(); i!=constraints.end(); ++i ){
        printf(" evaluating %d constraints in list\n", i->size());
        // disjunctive list A OR B OR C... etc
        bool inner = false;
        for ( j=i->begin(); j!=i->end(); ++j ){
            if (j->evaluate()) {
                inner = true;
                printf("pass\n");
                break;
            }
        }
        
		if ( !inner ) {
            printf("fail\n");
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
