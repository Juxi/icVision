#include "linearconstraint.h"
#include "bodypart.h"

using namespace KinematicModel;

LinearConstraint::LinearConstraint( BodyPart *_parent,
                 QStringList _a,
                 QStringList _q,
                 qreal _b,
                 bool _negate ) : parent(_parent), b(_b), negate(_negate)
{
    QStringList::iterator i,j;
    for ( i=_a.begin(), j=_q.begin(); i!=_a.end() && j!=_q.end(); ++i, ++j)
    {
        a.append( (*i).toDouble() );
        q.append( (*j).toInt() );
    }
}

bool LinearConstraint::evaluate()
{
    if ( !parent || a.begin()==a.end() || q.begin()==q.end() ) {
        printf("Bad constraint encountered!\n");
        return false;
    }
    
    //QString thisConstraint("Body Part ");
    //thisConstraint.append(QString::number(parent->index()));
    //thisConstraint.append(": ");
    
    double AdotQ = 0.0;
    QList<qreal>::iterator i;
    QList<int>::iterator j;
    for ( i=a.begin(), j=q.begin(); i!=a.end() && j!=q.end(); ++i, ++j)
    {
        if ( parent->at(*j) )
        {
            AdotQ += (*i) * parent->at(*j)->encPos();
            //thisConstraint.append(QString::number(*i));
            //thisConstraint.append(" * ");
            //thisConstraint.append(QString::number(parent->at(*j)->encPos()));
            //thisConstraint.append(" ");
        }
    }
    
    double ans = AdotQ + b;
    bool result = ans > 0;
    if (negate) result = !result;
    
    //if (negate) thisConstraint.append("< 0");
    //else thisConstraint.append("> 0");
  
    //if (!result) printf("%s FAILED... reflex initiated\n", thisConstraint.toStdString().c_str());
    return result;
}
