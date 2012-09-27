#include "link.h"
//#include "displmatrix.h"

using namespace KinematicModel;

Link::Link(  /*DT_RespTableHandle robotTable,*/ Robot* robot, int part, KinTreeNode* parent ) : KinTreeNode(/*robotTable,*/ robot, part, parent ,LINK)
{
}
Link::~Link()
{
}
void Link::setM() {
	M.setPosition(nodeAxis);
}
