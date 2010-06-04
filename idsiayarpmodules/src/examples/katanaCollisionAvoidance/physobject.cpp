#include "physobject.h"
#include <iostream>

using namespace std;

PhysObject::PhysObject(): solidObject(0), displayListIdx(0)
{
    txfrToLinkCS.setToIdentity();
    txfrToWorldCS.setToIdentity();
}
PhysObject::~PhysObject(){}
int PhysObject::displayList() { return displayListIdx; }
DtObjectRef* PhysObject::solid() { return &solidObject; }
const QMatrix4x4& PhysObject::txfrToWorld() const { return txfrToWorldCS; }
void PhysObject::updateTxfrToWorld( const QMatrix4x4& txfrLinkToWorld )
{
    txfrToWorldCS = txfrLinkToWorld * txfrToLinkCS;
    dtSelectObject( &solidObject );
    dtLoadIdentity();
    dtMultMatrixd( txfrToWorldCS.constData() );

    //const qreal* t = txfrToWorldCS.constData();
    //cout << "---------------------------------------------------" << endl;
    //cout << "updated a SOLID cylinder" << endl;
    //cout << "---------------------------------------------------" << endl
    //     << "txfrToWorldCS:  " << t[0] << " " << t[4] << " " << t[8]  << " " << t[12] << endl
    //     << "                " << t[1] << " " << t[5] << " " << t[9]  << " " << t[13] << endl
    //     << "                " << t[2] << " " << t[6] << " " << t[10] << " " << t[14] << endl
    //     << "                " << t[3] << " " << t[7] << " " << t[11] << " " << t[15] << endl;
}
