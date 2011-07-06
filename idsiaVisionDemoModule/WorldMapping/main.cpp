#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <string>

//Get all OS and signal processing YARP classes
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>
#include <yarp/sig/Matrix.h>

#include "WorldMapping.h"

YARP_DECLARE_DEVICES(icubmod)

using namespace yarp::os;
using namespace yarp::sig;
using namespace std;


int main(int argc, char * argv[]) {

  Network yarp; // set up yarp
  WorldMapping module;


  cout<<"Launching World Mapping Module"<<endl;
  //putenv("VISION_ROOT=/home/icub/IM-CLeVeR/idsiaVisionDemoModule/WorldMapping/conf");
  putenv("VISION_ROOT=/home/migliore/Dropbox/Projects/IM-CLeVeR/idsiaVisionDemoModule/WorldMapping/conf");

  ResourceFinder rf;
  rf.setVerbose(true);
  rf.setDefaultConfigFile("worldmap.ini");
  rf.configure("VISION_ROOT", argc, argv);

  if(!yarp.checkNetwork()){

      cerr<<"Yarp Network not available"<<endl;
      return true;

  }

  YARP_REGISTER_DEVICES(icubmod);

  return module.runModule(rf);

}


