#include "linearconstraint.h"
#include "bodypart.h"

using namespace KinematicModel;

LinearConstraint::LinearConstraint( BodyPart *_parent,
                                    QStringList _a,
                                    QStringList _q,
                                    qreal _b,
                                    bool _negate ) : parent(_parent),
                                                    b(_b),
                                                    negate(_negate)
{
    double aLen = 0.0;
    QStringList::iterator i,j;
    for ( i=_a.begin(), j=_q.begin(); i!=_a.end() && j!=_q.end(); ++i, ++j)
    {
        aLen += (*i).toDouble()*(*i).toDouble();
        a.append( (*i).toDouble() );
        q.append( (*j).toInt() );
    }
    aLen = sqrt(aLen);
    
    if (negate) {
        for ( i=_a.begin(); i!=_a.end(); ++i )
            norm.append(-(*i).toDouble()/aLen);
    } else {
        for ( i=_a.begin(); i!=_a.end(); ++i )
            norm.append((*i).toDouble()/aLen);
    }
}

bool LinearConstraint::evaluate(QVector<qreal>& springDispl)
{
    if ( !parent || a.begin()==a.end() || a.size()!=norm.size() || a.size()!=q.size() ) {
        printf("Bad constraint encountered!\n");
        return false;
    }

    double AdotQ = 0.0;
    QList<qreal>::iterator i;
    QList<int>::iterator j;
    for ( i=a.begin(), j=q.begin(); i!=a.end() && j!=q.end(); ++i, ++j) {
        if ( parent->at(*j) )
            AdotQ += (*i) * parent->at(*j)->encPos();
    }

    double ans = AdotQ + b;
    bool result = ans > 0;
    if (negate) {
        result = !result;
        ans = -ans;
    }
   
    springDispl.clear();
    for ( i=norm.begin(), j=q.begin(); i!=norm.end() && j!=q.end(); ++i, ++j)
        springDispl.append(ans*(*i));

    return result;
}
