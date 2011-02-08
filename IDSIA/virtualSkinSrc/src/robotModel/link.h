#ifndef LINK_H
#define LINK_H


#include "kintreenode.h"

class Robot;

/*! \brief Represents a link in the robot
 *
 */

class Link : public KinTreeNode
{
public:
    Link( Robot* robot, KinTreeNode* parent );
    //Link( KinTreeNode* parent );
    ~Link();

    //virtual bool setAxis( const QVector3D& vector );

private:
    virtual void setM();
};

#endif // LINK_H
