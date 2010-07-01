#include "link.h"
#include "displmatrix.h"

Link::Link( Robot* robot ) : KinTreeNode(robot,LINK)
{
}
Link::Link( KinTreeNode* parent ) : KinTreeNode(parent,LINK)
{
}
Link::~Link()
{
    //cout << "delete( Link" << index << " )..." << endl;
    //QVector<Link*>::iterator i;
    //for ( i=children.begin(); i!=children.end(); ++i ) {
    //    delete (*i);
    //}
}
void Link::setM() { M = DisplMatrix( QVector3D(), nodeAxis, 0, nodeAxis.length()); }
