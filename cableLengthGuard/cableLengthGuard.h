#include <math.h>

#include <yarp/os/all.h>
#include <yarp/math/Math.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>

#include <stdio.h>
#include <string.h>
#include <iostream>

//getting last command
#include "collectorPort.h"

using namespace std;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;
using namespace yarp::dev;

const char cableNames[8] = {'a', 'b', 'g', 'd', 'r', 'p', 'P', 'c'};
const int NUMBER_OF_CONSTRAINTS = 8;
const int CABLE_WARNING = 20;

class cableLengthGuard: public RateThread
{
public:
  cableLengthGuard(PolyDriver *, collectorPort *, Matrix, Vector, int );
  ~cableLengthGuard();
  bool threadInit();
  void setThetaMap(int *);
  void threadRelease();
  void run();
  void fixedTimeMove(const double*, const double*, double);
    
private:
  PolyDriver *Arm_dd;
	
  IPositionControl *pos;
  IVelocityControl *vel;
  IEncoders *enc;
  IPidControl *pid;
  IAmplifierControl *amp;
  IControlLimits *lim;

  int numberOfJoints;
  double *theta;
  int    *thetaMap;
  double **cableLength;

  collectorPort *lastCommandPos;

  Matrix C;
  Vector d;
};