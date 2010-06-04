#include <iostream>
#include "robot.h"

using namespace std;

Branch::Branch( Robot* robot, Branch* parentBranch ) : index(robot->branchList.size()), parentBranch(parentBranch)
{
    robot->branchList.append(this);
}
Branch::~Branch()
{
    cout << "delete( branch" << this->idx() << " )..." << endl;
    //QVector<Motor*>::iterator i;
    //for ( i=begin(); i!=end(); ++i ) {
    //    delete (*i);
    //}
}

// public
int Branch::idx() const { return index; }
const QString& Branch::name() const { return branchName; }
Branch* Branch::parent() const { return parentBranch; }
bool Branch::setEncPos( const QVector<qreal>& x )
{
    if ( x.size() > size() ) {
//        cout << "ERROR Branch " << idx() << ": " << name().toStdString() << endl
//             << "Position vector size exceeded. size > " << size() << endl;
//        return 0;
    } else if ( x.size() < size() ) {
        cout << "WARNING Branch " << idx() << ": " << name().toStdString() << endl
             << "Position vector size < " << size() << ". Some motor positions will not be set." << endl;
    }
    QVector<Motor*>::iterator i;
    QVector<qreal>::const_iterator j;
    int count = 0;
    for ( i=begin(),j=x.begin(); j!=x.end(); ++i,++j ) {
    	count++;
    	if (count > 3) {break;}
        (*i)->setEncPos(*j);
    }
    return 1;
}

// private
void Branch::setName(const QString& name) { branchName = name; }
