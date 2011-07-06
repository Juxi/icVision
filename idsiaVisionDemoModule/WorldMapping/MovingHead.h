/*
 * MovingHead.h
 *
 *  Created on: Jul 4, 2011
 *      Author: icub
 */

#ifndef MOVINGHEAD_H_
#define MOVINGHEAD_H_

//YARP
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

#include <opencv2/opencv.hpp>

class MovingHead {

private:


	std::string name2DPointLeftInPort;
    std::string name2DPointRightInPort;
    yarp::os::BufferedPort<yarp::os::Bottle > Point2DLeftInPort;
    yarp::os::BufferedPort<yarp::os::Bottle > Point2DRightInPort;
    bool receive2DPntLeft;
    bool receive2DPntRight;
	yarp::os::Bottle *Pnt2DLeftIn;
	yarp::os::Bottle *Pnt2DRightIn;

	/*
	 *
	std::string name3DPointLeftInPort;
    std::string name3DPointRightInPort;
    yarp::os::BufferedPort<yarp::os::Bottle > Point3DLeftInPort;
    yarp::os::BufferedPort<yarp::os::Bottle > Point3DRightInPort;
    bool receive3DPntLeft;
    bool receive3DPntRight;
	yarp::os::Bottle *Pnt3DLeftIn;
	yarp::os::Bottle *Pnt3DRightIn;
	 *
	 */





public:
	MovingHead();
	virtual ~MovingHead();
	bool interrupt();
	bool close();
	bool get2DPoints();
	bool get3DPoints();
	bool connect(std::string port2connectleft,std::string port2connectright);
};

#endif /* MOVINGHEAD_H_ */
