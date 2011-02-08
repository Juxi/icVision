#ifndef JOINT_H
#define JOINT_H

//#include <QtCore>
#include "interval.h"
#include "kintreenode.h"

class Motor;

class Joint : public KinTreeNode
{
public:
    Joint( Robot* robot, KinTreeNode* parent, Motor* motor, Type type );
    //Joint( KinTreeNode* parent, Motor* motor, Type type );
    virtual ~Joint();

    // set stuff
    void setMin( qreal pos ) { limits.setMin(pos); }
    void setMax( qreal pos ) { limits.setMax(pos); }
    void setPos();
    //virtual bool setAxis( const QVector3D& vector );

protected:
    Motor*   motor;

    qreal    position;      // joint position
    Interval limits;

    virtual void setM() = 0;
};

#endif // JOINT_H
