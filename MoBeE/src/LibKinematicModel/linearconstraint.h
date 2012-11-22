/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RobotModel
 *	@{
 */

#ifndef LINEARCONSTRAINT_H
#define LINEARCONSTRAINT_H

#include <QStringList>
#include <QVector>


namespace KinematicModel
{
	class LinearConstraint;
    class BodyPart;
}

// linear constraints a*q > d
class KinematicModel::LinearConstraint {
public:
    LinearConstraint(){}
    LinearConstraint( BodyPart *_parent,
                     QStringList _a,
                     QStringList _q,
                     qreal _b);
    ~LinearConstraint(){}
    
    QVector<qreal> getNorm();
    bool evaluate(qreal&);
    
private:
    BodyPart		*parent;
    QList<qreal>	a;
    QList<int>		q;
    qreal			b;
    qreal           aLen;
};
#endif

/** @} */
