
#ifndef FITNESS_H
#define FITNESS_H


#include "fwdkin.h"
#include "fitnessfunction.h"


// special sigmoid
double functionValueToJointAngle(
		double zeroPosition,
		double jointLimitLower,
		double jointLimitUpper,
		double functionValue);

void functionValuesToJointAngles(
		unsigned int numSegments,
		const double* zeroPosition,
		const double* lowerLimit,
		const double* upperLimit,
		unsigned int numPoses,
		const double* functionValues,
		double* jointAngles);

double fitnessArm(
		unsigned int numSegments,
		const double* zeroPosition,
		const double* lowerLimit,
		const double* upperLimit,
		FwdKin& kinematics,
		unsigned int numTimesteps,
		const double* functionValues);

extern double    zeropos[7];
extern double lowerLimit[7];
extern double upperLimit[7];

extern double goalPos[3];

// fitness encapsulation for xNES
class ICubFitness : public FitnessFunction
{
public:
	ICubFitness(FwdKin& fwdkin, unsigned int lvl = 1);
	~ICubFitness();

	double eval(Vector const& point) const;

protected:
	unsigned int level;
	FwdKin& kinematics;
	std::vector<double> time;
};


#endif
