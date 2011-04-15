/*******************************************************************
 ***             Copyright (C) 2011 Tobias Glasmachers           ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

#include <math.h>
#include <QThread>
#include "fwdkin.h"
#include "fitness.h"


#define RIGHT_ARM 2


class SleepHelperUglyQtWorkaroundShit : public QThread
{
public:
	static void msleep(unsigned int msec)
	{
		QThread::msleep(msec);
	}
};


FwdKin::FwdKin(bool visualize)
: model(visualize)
, robot(*model.robot)
, world(*model.world)
{
	// add obstacles to the world
	QVector3D pos(-0.2, 0.15, 0.15);
	world.newSphere(0.05, pos);

	if (visualize)
	{
		// add a little "target" sphere
		QVector3D pos(goalPos[0], goalPos[1], goalPos[2]);
		world.newSphere(0.01, pos);
	}
}

FwdKin::~FwdKin()
{ }


void FwdKin::pose(const double* jointAngle, bool& collision, double* handpos, bool wait)
{
	unsigned int i;
	for (i=0; i<7; i++) robot.setEncoderPosition(RIGHT_ARM, i, jointAngle[i]);
	m_hasObs = false;
	collision = (model.computePose() > 0);
	if (! m_hasObs)
	{
		if (wait)
		{
			while (! m_hasObs) SleepHelperUglyQtWorkaroundShit::msleep(1);
		}
		else throw "[FwdKin::pose] no observation available -- maybe Qt::connect is queued?";
	}
	QMatrix4x4 const& matrix = m_obs.markerConfiguration(0);
	handpos[0] = matrix(0, 3);
	handpos[1] = matrix(1, 3);
	handpos[2] = matrix(2, 3);
}

void FwdKin::obtainObservation(RobotObservation obs)
{
	m_obs = obs;
	m_hasObs = true;
}
