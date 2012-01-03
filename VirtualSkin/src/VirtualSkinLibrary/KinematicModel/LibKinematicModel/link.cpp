#include "link.h"
//#include "displmatrix.h"

using namespace KinematicModel;

Link::Link(  /*DT_RespTableHandle robotTable,*/ Robot* robot, KinTreeNode* parent ) : KinTreeNode(/*robotTable,*/ robot, parent ,LINK)
{
}
Link::~Link()
{
}
void Link::setM() {
	M.setPosition(nodeAxis);
}
