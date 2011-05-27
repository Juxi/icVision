// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <stdio.h>
#include <stdlib.h>
#include "saliency_module.h"

using namespace yarp::os;
using namespace yarp::sig;
using namespace std;


int main(int argc, char * argv[]) {

  Network yarp;
  SaliencyModule module;

  cout<<"Launching IM-CLeVeR Saliency Module ..."<<endl;
  /* run the module: runModule() calls configure first and, if successful, it then runs */
  module.runModule(argc, argv);
  module.close();

  return 0;
}


