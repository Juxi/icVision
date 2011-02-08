#include "link.h"
#include "displmatrix.h"

Link::Link( Robot* robot, KinTreeNode* parent ) : KinTreeNode(robot, parent ,LINK)
{
}
Link::~Link()
{
}
void Link::setM() {
	M.setTranslation(nodeAxis);
}
