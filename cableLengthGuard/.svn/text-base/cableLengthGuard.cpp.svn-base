#include "cableLengthGuard.h"

cableLengthGuard::cableLengthGuard(PolyDriver *Arm_d, collectorPort *lastCommand, Matrix Cin, Vector din, int rate): RateThread(rate)
  {
    // open ports
    Arm_dd=Arm_d;

    bool ok;
    ok  = Arm_dd->view(pos);
    ok &= Arm_dd->view(vel);
    ok &= Arm_dd->view(enc);
    ok &= Arm_dd->view(pid);
    ok &= Arm_dd->view(amp);
    ok &= Arm_dd->view(lim);

    if (!ok)
      fprintf(stderr, "Problems acquiring interfaces\n");
    else
      fprintf(stderr, "Control board was accessed succesfully!\n");

    //store used joints
    numberOfJoints = Cin.cols();
    
    //initialize used variables
    theta = new double [numberOfJoints];
    thetaMap = new int [numberOfJoints];
    cableLength = new double* [numberOfJoints];
    for (int i = 0; i < numberOfJoints; i++)
      cableLength[i] = new double [NUMBER_OF_CONSTRAINTS];

    //initialize collectorPort
    lastCommandPos = lastCommand;
    
    //define the inequalities
    //add a fake lines for CQP notation
    C.resize(Cin.rows(),Cin.cols()+1);
    C.zero();
    for (int i = 0; i < Cin.rows(); i++)
      for (int j = 0; j < Cin.cols(); j++)
	C(i,j) = Cin(i,j);
   
    d.resize(din.size());
    d = din;
  }

cableLengthGuard::~cableLengthGuard()
  {    }

void cableLengthGuard::setThetaMap(int *map)
  { 
    fprintf(stderr, "Setting the map\n");
    for (int i = 0; i < numberOfJoints; i++)
	thetaMap[i] = map[i];
  }

bool cableLengthGuard::threadInit()
{

  return 1;
}

void cableLengthGuard::threadRelease()
{
  for (int i = 0; i < numberOfJoints; i++)
    delete[] cableLength[i];
  delete[] theta;
  delete[] thetaMap;

  fprintf(stderr, "Closing device driver \n");
  Arm_dd->close();  
}

void cableLengthGuard::run()
{
  //ACE_OS::printf("Entering the main thread\n");
  for (int i = 0; i < numberOfJoints; i++)
    enc->getEncoder(thetaMap[i], theta+i);
            
  Vector thetaDes(numberOfJoints);

  //get the last commanded position
  //if non commanded position is available
  //get current position
  for(int i = 0; i < numberOfJoints; i++)
    {
      if (!lastCommandPos->get(thetaDes(i), thetaMap[i]))
	thetaDes(i) = theta[i];
    }

  Vector  ThetaCqp(4);
  ThetaCqp(0) = thetaDes(0);
  ThetaCqp(1) = thetaDes(1);
  ThetaCqp(2) = thetaDes(2);
  ThetaCqp(3) = 0;         //fake variable for CQP

  Vector jointLimits(11);
  jointLimits = C*ThetaCqp - d;
  //fprintf(stderr, "jointLim is %s\n", jointLimits.toString().c_str());
  
  bool stopMovement = false;
  for(int i = 0; i < 5; i++) 
    if (jointLimits(i) - CABLE_WARNING < 0)
	{
		fprintf(stderr, "%d ", i);
		stopMovement = true;
	}

  if(stopMovement)
  {
	fprintf(stderr, "Stopping movement in the current configuration \n");
	//for (int i = 0; i < numberOfJoints; i++)
	//	pos->positionMove(thetaMap[i], theta[i]);

	Time::delay(0.1);
    lastCommandPos->reset();
  }
}  


  //Let's compute the pulleys rotation angles
  //[1] refers to the the angles relative to joint1
  //[2] refers to the the angles relative to joint2
  //alpha[1] = -44 + (-theta[0] + theta[1])/b;
  //alpha[2] = 19 + (-theta[0] + theta[1] + theta[2])/b;
            
  //beta[1] = (-alpha[1] + 171) * b;
  //beta[2] = (-alpha[2] + 153) * b;

  //gamma[1] = alpha[1]  + 180;    //this is a little conservative
  //gamma[2] = alpha[2]  + 180;

  //delta[1] = beta[1]  + 90;
  //delta[2] = beta[2]  + 90;      //this is a little conservative

  //rho[1] = beta[1]  + 90;
  //rho[2] = beta[2]  + 210;            

  //phi[1] =-theta[1]  + 210;
  //phi[2] =-theta[2]  + 120;

  //psi[1] = rho[1]+90;          //this is a little conservative
  //psi[2] = rho[2]-60;  

  //csi[1] = phi[1];
  //csi[2] =-theta[2]  + 240; 

  //alpha[1] - 360         < 0     alpha[2] - 450         < 0
  //beta[1]  - 180         < 0     beta[2]  - 180         < 0
  //gamma[1] - 90          > 0     gamma[2] - 90          > 0
  //delta[1] + 90          > 0     delta[2] + 90          > 0
  //rho[1]   - theta[0]    > 0     rho[2] - theta[0] - 360< 0
  //phi[1]   - 360         < 0     phi[2]                 > 0
  //psi[1] - theta[0] -360 < 0     psi[2] - theta[0]      > 0
  //csi[1]                 > 0     csi[2] -360            < 0

  //let's compute the cable length on the pulleys
  //cableLength[1][0] =  - alpha[1] + 360;
  //cableLength[1][1] =  - beta[1]  + 180;
  //cableLength[1][2] =    gamma[1] - 90;
  //cableLength[1][3] =    delta[1] + 90;              
  //cableLength[1][4] =      rho[1] - theta[0];
  //cableLength[1][5] =  - phi[1]   + 360;
  //cableLength[1][6] =  - psi[1]   + theta[0] + 360;
  //cableLength[1][7] = csi[1];

  //cableLength[2][0] =  - alpha[2] + 450;
  //cableLength[2][1] =  - beta[2]  + 180;
  //cableLength[2][2] =    gamma[2] - 90;
  //cableLength[2][3] =    delta[2] + 90;
  //cableLength[2][4] =  - rho[2]   + theta[0] +360;
  //cableLength[2][5] =    phi[2];
  //cableLength[2][6] =      psi[2] - theta[0];
  //cableLength[2][7] =    - csi[2] + 360;

  //print a warning in case a cable length is shortening too much
  //for (int i = 1; i < 3; i++)
  //    for (int j = 0; j < 8; j++)
  //        if (cableLength[i][j] < cableWarning)
  //            ACE_OS::printf ("%c%d:(*)%.1f ", cableNames[j], i, cableLength[i][j]);
  //        else
  //            ACE_OS::printf ("%c%d:%.1f ", cableNames[j], i, cableLength[i][j]);

  //ACE_OS::printf ("\n");

  //and refromulate the contraints in a different way
  //cableLength[1][0] = 347.0       + 65.0/38.0 * theta[0] - 65.0/38.0 * theta[1]; //
  //cableLength[1][1] = 732.0/13.0  -             theta[0] +             theta[1];
  //cableLength[1][2] = 939.0/19.0  - 65.0/38.0 * theta[0] + 65.0/38.0 * theta[1];
  //cableLength[1][3] = 3948.0/13.0 +             theta[0] -             theta[1];
  //cableLength[1][4] = 2609.0/13.0                        -             theta[1];
  //cableLength[1][5] = 165.0                              +             theta[1];
  //cableLength[1][6] = 901.0/13.0                         +             theta[1];
  //cableLength[1][7] = 195.0                              -             theta[1];

  //cableLength[2][0] = 366.57      + 65.0/38.0 * theta[0] - 65.0/38.0 * theta[1] - 65.0/38.0 * theta[2]; //
  //cableLength[2][1] = 6738.0/65.0 -             theta[0] +             theta[1] +             theta[2];
  //cableLength[2][2] = 2136.0/19.0 - 65.0/38.0 * theta[0] + 65.0/38.0 * theta[1] + 65.0/38.0 * theta[2]; //
  //cableLength[2][3] = 16662.0/65.0+             theta[0] -             theta[1] -             theta[2];
  //cableLength[2][4] = 66.6        +                                    theta[1] +             theta[2]; //
  //cableLength[2][5] = 120.0                                                     -             theta[2];
  //cableLength[2][6] = 13867.0/65.0                       -             theta[1] -             theta[2]; //
  //cableLength[2][7] = 120.0                                                     +             theta[2];

  //print a warning in case a cable length is shortening too much
  //for (int i = 1; i < 3; i++)
  //  for (int j = 0; j < 8; j++)
  //    if (cableLength[i][j] < CABLE_WARNING)
  //	ACE_OS::printf ("%c%d:(*)%.1f ", cableNames[j], i, cableLength[i][j]);
  //    else
  //	ACE_OS::printf ("%c%d:%.1f ", cableNames[j], i, cableLength[i][j]);
            
  //ACE_OS::printf ("\n");
  //ACE_OS::printf("Exiting the main thread\n");
  //ACE_OS::printf ("\r");

