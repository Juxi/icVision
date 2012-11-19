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
	//int count = 0;
    QVector< QVector< QPair< qreal, QVector<qreal> > > > newlyEvaluatedConstraints;
	QVector< QVector<LinearConstraint> >::iterator i;
    QVector<LinearConstraint>::iterator j;
    bool result = true;
	for ( i=constraints.begin(); i!=constraints.end(); ++i )
    {
        bool inner = false;
        QVector<qreal> thisSpring;
        QVector< QPair< qreal, QVector<qreal> > > thisEvaluatedConstraint;
        
        // disjunctive list A OR B OR C... etc
        for ( j=i->begin(); j!=i->end(); ++j )
        {
            qreal springVal;
            if (j->evaluate(springVal))
                inner = true;
            thisEvaluatedConstraint.append( QPair< qreal, QVector<qreal> >(springVal, j->getNorm()) );
        }
        newlyEvaluatedConstraints.append(thisEvaluatedConstraint);
		if ( !inner ) result = false;
	}
    evaluatedConstraints = newlyEvaluatedConstraints;
    //printEvaluatedConstraints();
	return result;
}

void BodyPart::printEvaluatedConstraints(){
    QVector< QVector< QPair< qreal, QVector<qreal> > > >::iterator i;
    QVector< QPair< qreal, QVector<qreal> > >::iterator j;
    QVector<qreal>::iterator k;
    printf("-----------------------------------------------------\n");
    for (i=evaluatedConstraints.begin(); i!=evaluatedConstraints.end(); ++i){
        printf("Set of constraints...\n");
        for (j=i->begin(); j!=i->end(); ++j) {
            printf("  constraint: %f (",j->first);
            int count = 0;
            for (k=j->second.begin(); k!=j->second.end(); ++k) {
                if (count<7) printf(" %f",*k);
                count++;
            }
            printf(")\n");
        }
    }
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
    QVector<qreal> t;
    QVector<Motor*>::iterator i;
    for ( i=begin(); i!=end(); ++i )
        t.append((*i)->getTorque());
    emit repulsiveForce(t);
    emit constraintState(evaluatedConstraints);
}
