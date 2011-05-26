
#include <stdio.h>
#include <stdlib.h>
#include "RecordModule.h"

using namespace yarp::os;
using namespace yarp::sig;
using namespace std;


int main(int argc, char * argv[]) {


  Network yarp; // set up yarp
  RecordModule module;

  cout<<"Launching ImageSequenceRecorder"<<endl;
  /* run the module: runModule() calls configure first and, if successful, it then runs */
  module.runModule(argc,argv);
  //yarp::os::ResourceFinder rf;
  //module.runModule(rf);

  module.close();

  return 0;

}


