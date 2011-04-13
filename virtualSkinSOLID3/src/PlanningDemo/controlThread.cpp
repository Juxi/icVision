/*******************************************************************
 ***             Copyright (C) 2011 Tobias Glasmachers           ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

#include "math.h"
#include "controlThread.h"
#include "fitness.h"
#include "kernelfunction.h"


ControlThread::ControlThread(FwdKin& fwdkin, const double* coeff)
: m_kinematics(fwdkin)
, m_coeff(coeff)
, m_time(0)
{
	double time[101];
	double f[707];
	unsigned int t;
	for (t=0; t<=100; t++) time[t] = 0.01 * t;
	evalFunctions(7, 101, 1, m_coeff, &time[0], f);
	functionValuesToJointAngles(7, zeropos, lowerLimit, upperLimit, 101, f, &m_j[0][0]);
}

void ControlThread::run()
{
	try
	{
		for (m_time=0; ; m_time++)
		{
			unsigned int time = (std::max(std::min(m_time, 150u), 50u) - 50);
			if (m_time == 200) m_time = 0;
// 			printf("time=%g\n", time);

			// set pose for the given time
			bool collision;
			double handPos[3];
			m_kinematics.pose(m_j[time], collision, handPos, true);

			msleep(20);
		}
	}
	catch (const char* exception)
	{
		printf("exception: %s\n", exception);
	}
}
