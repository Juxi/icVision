#include "linearconstraint.h"
#include "bodypart.h"

using namespace KinematicModel;

LinearConstraint::LinearConstraint( BodyPart *_parent,
                                    QStringList _a,
                                    QStringList _q,
                                    qreal _b,
                                    //qreal _nogo,
                                    bool _negate ) : parent(_parent),
                                                    b(_b),
                                                    //nogo(_nogo),
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
    nogo = 20.0;
    
    if (negate) {
        for ( i=_a.begin(); i!=_a.end(); ++i )
            norm.append(-(*i).toDouble()/aLen);
    } else {
        for ( i=_a.begin(); i!=_a.end(); ++i )
            norm.append((*i).toDouble()/aLen);
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
    
    if (qAbs(ans) < nogo) {
        for ( i=norm.begin(), j=q.begin(); i!=norm.end() && j!=q.end(); ++i, ++j)
        {
            double t = nogo-qAbs(ans);
           //parent->at(*j)->addExtTorque(t*t*(*i));
        }
    }
    
    //if (negate) thisConstraint.append("< 0");
    //else thisConstraint.append("> 0");
  
    //if (result) printf("PASSED...\n");
    //else printf("FAILED...\n");
    return result;
}
