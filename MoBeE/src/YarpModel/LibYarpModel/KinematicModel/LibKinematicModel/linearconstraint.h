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


namespace KinematicModel
{
	class LinearConstraint;
    class BodyPart;
}

// linear constraints C*q > d
class KinematicModel::LinearConstraint {
public:
    LinearConstraint(){}
    LinearConstraint( BodyPart *_parent,
                     QStringList _a,
                     QStringList _q,
                     qreal _b,
                     bool _negate = false);
    ~LinearConstraint(){}
    
    bool evaluate();
    
private:
    BodyPart		*parent;
    QList<qreal>	a;
    QList<int>		q;
    qreal			b;
    bool            negate;
};
#endif

/** @} */
