/*******************************************************************
 ***             Copyright (C) 2011 Tobias Glasmachers           ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

#include "fitness.h"
#include "fwdkin.h"
#include "fitnessfunction.h"
#include "kernelfunction.h"


// special sigmoid
double functionValueToJointAngle(
		double zeroPosition,
		double jointLimitLower,
		double jointLimitUpper,
		double functionValue)
{
	if (functionValue < 0.0)
	{
		double d = zeroPosition - jointLimitLower;
		if (d == 0.0) return zeroPosition;
		else return zeroPosition + d * tanh(functionValue / d);
	}
	else
	{
		double d = jointLimitUpper - zeroPosition;
		if (d == 0.0) return zeroPosition;
		else return zeroPosition + d * tanh(functionValue / d);
	}
}


void functionValuesToJointAngles(
		unsigned int numSegments,
		const double* zeroPosition,
		const double* lowerLimit,
		const double* upperLimit,
		unsigned int numPoses,
		const double* functionValues,
		double* jointAngles)
{
	unsigned int s, p;
	for (p=0; p<numPoses; p++)
	{
		for (s=0; s<numSegments; s++)
		{
			*jointAngles = functionValueToJointAngle(
					zeroPosition[s],
					lowerLimit[s],
					upperLimit[s],
					*functionValues);
			functionValues++;
			jointAngles++;
		}
	}
}


////////////////////////////////////////////////////////////
// Fitness function
//
double fitnessArm(
		unsigned int numSegments,
		const double* zeroPosition,
		const double* lowerLimit,
		const double* upperLimit,
		FwdKin& kinematics,
		unsigned int numTimesteps,
		const double* functionValues)
{
	double collision_penalty = 1e10;
	double goal_distance_penalty = 1.0;

	double fitness = 0.0;
	unsigned int t, i;

	// compute (bounded) joint angles from (unbounded) function values
	std::vector<double> jointAngles((numTimesteps + 1) * numSegments);
	functionValuesToJointAngles(
			numSegments,
			zeroPosition,
			lowerLimit,
			upperLimit,
			numTimesteps + 1,
			functionValues,
			&jointAngles[0]);

	// loop through all poses and test for collision
	bool collision;
	double handPos[3];
	for (t=0; t<=numTimesteps; t++)
	{
		kinematics.pose(&jointAngles[t * numSegments], collision, handPos);
		if (collision) fitness += collision_penalty;
	}

	// final configuration - penalize distance to the goal
	{
		double norm2 = 0.0;
		for (i=0; i<3; i++)
		{
			double diff = goalPos[i] - handPos[i];
			norm2 += diff * diff;
		}
		fitness += goal_distance_penalty * sqrt(norm2);
	}

	// return the fitness value
	return fitness;
}


double    zeropos[7] = {-26.0,  20.0,   0.0,  50.0,   0.0, -15.0,   0.0};
double lowerLimit[7] = {-96.0,   0.0, -37.0,  15.0, -90.0, -90.0, -20.0};
double upperLimit[7] = { 10.0, 161.0,  80.0, 106.0,  90.0,   0.0, -40.0};


double goalPos[3] = {-0.2, 0.2, 0.3};


ICubFitness::ICubFitness(FwdKin& fwdkin, unsigned int lvl)
: kinematics(fwdkin)
, level(lvl)
{
	time.resize(101);
	unsigned int t;
	for (t=0; t<=100; t++) time[t] = 0.01 * t;
}

ICubFitness::~ICubFitness()
{ }


double ICubFitness::eval(Vector const& point) const
{
	// evaluate kernel-based functions
	std::vector<double> f(707);
	evalFunctions(7, 101, level, &point[0], &time[0], &f[0]);

	double fit = fitnessArm(
			7,
			zeropos,
			lowerLimit,
			upperLimit,
			kinematics,
			100,
			&f[0]);

	// kernel regularization
	double complexity = 0.0;
// 	unsigned int s, N = (1 << m_levels) - 1 + m_levels;
// 	for (s=0; s<SEGMENTS; s++)
// 	{
// 		Vector p = point.sub(s*N, N);
// 		complexity += p * (K * p);
// 	}
	return fit + 0.01 * complexity;
}
