#ifndef BODYPART_H
#define BODYPART_H

#include "zphandler.h"

class Robot;
class Motor;

/*! \brief To set the positions of several motors at once (see Motor, Joint, and Robot)
 *
 * This class facilitates grouping of the motors (and therefore joints) into subsystems, such that we can replicate
 * the YARP interface to the iCub (or another robot). This allows us to seamlessly wrap the YARP server and filter the
 * commands being sent to the robot, without the need to change the client side code at all. Consider the following:
 * The iCub robot is configured such that a control program connects to a YARP port, which offers interfaces to several
 * motors. For example, one could connect to an RPC port called 'torso', which offers three motor interfaces, roll, pitch
 * and yaw. A control program can set the motor positions (or read encoder values) individually, or alternatively it can
 * set all positions belonging to the group 'torso' with a command of the form setAllTorsoPositions(roll,pitch,yaw), and
 * the same is true for reading encoder positions. In this case 'torso' is a Branch, so named because its three motors
 * correspond to joints, which constitute a 'branch' of the kinematic tree.
 */

class BodyPart : public QVector<Motor*>
{
public:
    BodyPart( Robot* robot, BodyPart* bodyPart = 0 );
    ~BodyPart();

    //int idx() const { return index; }
    const QString& name() const { return partName; }
    BodyPart* parent() const { return parentPart; }

    void setName( const QString& name ) { partName = name; }
    bool setEncPos( const QVector<qreal>& x );
    //bool setNormPos( const QVector<qreal>& x );  // implement this later
	bool verify();

private:
    //int       index;
    BodyPart* parentPart;
    QString   partName;
};

#endif // BODYPART_H
