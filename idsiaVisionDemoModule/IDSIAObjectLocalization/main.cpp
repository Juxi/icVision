#include <qapplication.h>
#include <stdio.h>
#include <stdlib.h>
#include "VisionModule.h"

using namespace yarp::os;
using namespace yarp::sig;
using namespace std;

//To manage QT messages and avoid warnings
void myMessageOutput(QtMsgType type, const char *msg)
{
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s\n", msg);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s\n", msg);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s\n", msg);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s\n", msg);
        abort();
    }
}

int main(int argc, char * argv[]) {

  //Only for QT windows
  qInstallMsgHandler(myMessageOutput);

  //Initialize yarp Network
  Network yarp; // set up yarp
  //Initialize the module
  VisionModule module;

  cout<<"Launching IMCLEVER vision"<<endl;
  /* run the module: runModule() calls configure first and, if successful, it then runs */
  module.runModule(argc,argv);
  //yarp::os::ResourceFinder rf;
  //module.runModule(rf);

  //Close the module
  module.close();

  return 0;

}


