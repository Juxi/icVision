#include "link.h"
#include "displmatrix.h"

using namespace RobotModel;

Link::Link( Robot* robot, KinTreeNode* parent ) : KinTreeNode(robot, parent ,LINK)
{
}
Link::~Link()
{
}
void Link::setM() {
	M.setPosition(nodeAxis);
}
