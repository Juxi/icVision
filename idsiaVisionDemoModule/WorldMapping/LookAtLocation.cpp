/*
 * LookAtLocation.cpp
 *
 *  Created on: Jul 7, 2011
 *      Author: Davide Migliore
 * 	GPL goes here...
 */

#include "LookAtLocation.h"

using namespace std;


LookAtLocation::LookAtLocation()
{
  // TODO Auto-generated constructor stub

}

bool LookAtLocation::threadInit(){

  //initialize here variables
  printf("ControlThread:starting\n");

  Property optGaze("(device gazecontrollerclient)");
  optGaze.put("remote","/iKinGazeCtrl");
  optGaze.put("local","/WorldMapping/gaze_client");

  Property optHead;
  optHead.put("device", "remote_controlboard");
  optHead.put("remote", "/icub/head");
  optHead.put("local", "/WorldMapping/local/head");

  // specify the port which will be used
  // when tracking.
  inputLookAt3DPortName = "/WorldMapping/lookAtPoint3D:i";

  if (!MotionPort.open(inputLookAt3DPortName.c_str())) {
      cout << ": Unable to open port " << inputLookAt3DPortName << endl;

  }

  clientGaze = new PolyDriver;
  if (!clientGaze->open(optGaze))
    {
      delete clientGaze;
      return false;
    }

  // open the view
  clientGaze->view(igaze);

  // latch the controller context in order to preserve
  // it after closing the module
  // the context contains the tracking mode, the neck limits and so on.
  igaze->storeContext(&startup_context_id);


  head = new PolyDriver;
  if (!head->open(optHead))
    {
      delete head;
      return false;
    }

  // open the view
  head->view(iencs);
  head->view(ivel);
  head->view(ipos);

  // set trajectory time:
  igaze->setNeckTrajTime(1.5);
  igaze->setEyesTrajTime(1.0);

  // put the gaze in tracking mode, so that
  // when the head moves, the gaze controller
  // will compensate for it
  igaze->setTrackingMode(true);

  if ( (!iencs) || (!ivel) || (!ipos) )
    {
      printf("Cannot get interface to robot head\n");
      return false;
    }

  joints = 0;

  iencs->getAxes(&joints);

  encoders.resize(joints);
  tmp.resize(joints);
  commands_position.resize(joints);
  commands_velocity.resize(joints);
  setpoints.resize(joints);
  checkpos.resize(joints);


  velocityFactor = 0.5;

  fp.resize(3);

}

void LookAtLocation::threadRelease(){

  printf("LookAtLocation:stopping the robot\n");

  MotionPort.interrupt();
  MotionPort.close();

  // we require an immediate stop
  // before closing the client for safety reason
  // (anyway it's already done internally in the
  // destructor)
  igaze->stopControl();
  ivel->stop();
  ipos->stop();

  // it's a good rule to restore the controller
  // context as it was before opening the module
  igaze->restoreContext(startup_context_id);


  delete head;
  delete clientGaze;

  printf("\nDone, goodbye from LookAtLocation\n");
}

void LookAtLocation::run(){

  target=NULL;

  std::cout<<"I am here"<<std::endl;
  target = MotionPort.read(false);//.read(false)

  std::cout<<"I am here"<<std::endl;
  if (target != NULL && target->size()== 4)
    {
      x = target->get(0).asDouble();
      y = target->get(1).asDouble();
      z = target->get(2).asDouble();
      check = target->get(3).asString();

      if(check=="itworks")
        {
          fp[0] = x;
          fp[1] = y;
          fp[2] = z;
        }
      igaze->lookAtFixationPoint(fp); // move the gaze to the desired fixation point
      igaze->waitMotionDone();        // wait until the operation is done

      // some verbosity
      //printStatus();

    }


}

void LookAtLocation::onStop(){

}

/*
void LookAtLocation::printStatus(){

  Vector x;

  // we get the current fixation point in the
  // operational space
  igaze->getFixationPoint(x);

  fprintf(stdout,"+++++++++\n");
  fprintf(stdout,"fp         [m] = %s\n",fp.toString().c_str());
  fprintf(stdout,"x          [m] = %s\n",x.toString().c_str());
  fprintf(stdout,"norm(fp-x) [m] = %g\n",norm(fp-x));
  fprintf(stdout,"---------\n\n");

}
 */

void LookAtLocation::storeInterestingPoint()
{

  Vector ang;

  // we store the current azimuth, elevation
  // and vergence wrt the absolute reference frame
  // The absolute reference frame for the azimuth/elevation couple
  // is head-centered with the robot in rest configuration
  // (i.e. torso and head angles zeroed).



}

bool LookAtLocation::connect(std::string port2connect){
  std::cout<<port2connect.c_str()<<std::endl;
  std::cout<<inputLookAt3DPortName.c_str()<<std::endl;

  Network::connect(port2connect.c_str(),inputLookAt3DPortName.c_str());
}
