/************************************************************
 * Copyright: (C) 2011 Juxi Leitner	                    	*
 * Author: Juxi Leitner <juxi.leitner@gmail.com> @ IDSIA.ch *
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.*
 ************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "PlayModule.h"

int main(int argc, char * argv[]) {
    yarp::os::Network yarp; 
    PlayModule module;
    
    std::cout << "Launching ImageSequenceStreamer" << std::endl;
    module.runModule(argc, argv);
    module.close();
    
    return 0;
}


