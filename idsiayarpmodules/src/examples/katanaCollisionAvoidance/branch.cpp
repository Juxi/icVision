#include <iostream>
#include "robot.h"

using namespace std;

Branch::Branch( Robot* robot, Branch* parentBranch ) : index(robot->nextBranchIdx()), parentBranch(parentBranch)
{
    //robot->branchList.append(this);
    robot->appendBranch(this);
}
Branch::~Branch()
{
    cout << "delete( branch" << this->idx() << " )..." << endl;
    //QVector<Motor*>::iterator i;
    //for ( i=begin(); i!=end(); ++i ) {
    //    delete (*i);
    //}
}

bool Branch::setEncPos( const QVector<qreal>& x )
{
    if ( x.size() > size() ) {
	    cout << "ERROR Branch " << idx() << ": " << name().toStdString() << endl
             << "Position vector size exceeded. size > " << size() << endl;
        return 0;
    } else if ( x.size() < size() ) {
        cout << "WARNING Branch " << idx() << ": " << name().toStdString() << endl
             << "Position vector size < " << size() << ". Some motor positions will not be set." << endl;
    }
    QVector<Motor*>::iterator i;
    QVector<qreal>::const_iterator j;
	//int count = 0;
    for ( i=begin(),j=x.begin(); j!=x.end(); ++i,++j ) {
    //    if ( count < 6 ) 
			(*i)->setEncPos(*j);
	//	count++;
    }
    return 1;
}
