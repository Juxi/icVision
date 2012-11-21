#include "linearconstraint.h"
#include "bodypart.h"

using namespace KinematicModel;

LinearConstraint::LinearConstraint( BodyPart *_parent,
                                    QStringList _a,
                                    QStringList _q,
                                    qreal _b ) : parent(_parent),
                                                 b(_b)
{
    QStringList::iterator i,j;
    for ( i=_a.begin(), j=_q.begin(); i!=_a.end() && j!=_q.end(); ++i, ++j)
    {
        aLen += (*i).toDouble()*(*i).toDouble();
        a.append( (*i).toDouble() );
        q.append( (*j).toInt() );
    }
    aLen = sqrt(aLen);
}

QVector<qreal> LinearConstraint::getNorm(){
    QVector<qreal> norm;
    norm.resize(parent->size());
    QList<qreal>::iterator m;
    QList<int>::iterator n;
    for ( m=a.begin(), n=q.begin(); m!=a.end() && n!=q.end(); ++m, ++n) {
        //printf("replace(%d,%f)\n",*n,*m/aLen);
        norm.replace(*n, *m/aLen);
    }
    return norm;
}

bool LinearConstraint::evaluate(qreal& ans)
{
    if ( !parent || a.begin()==a.end() || a.size()!=q.size() ) {
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

    ans = AdotQ + b;

    return ans > 0;
}
