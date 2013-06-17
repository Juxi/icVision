#include <unistd.h>
#include <time.h>
#include "learner.h"
//#include "actionReach.h"
//#include "actionTransition.h"
#include "util.h"

int main(int argc, char *argv[])
{
    // read command line params
    yarp::os::Property config;
    config.fromCommand(argc,argv);
    
    bool connectToMoBeE = true;
    if ( config.check("dontConnect") )
        connectToMoBeE = false;
    
    // instantiate reinforcement learner
    Learner learner(16,"icubSim","right_arm",connectToMoBeE);
    
    // initialize from file
    bool initialized = false;
    if ( config.check("file") ) {
        std::string fileName = config.find("file").asString().c_str();
        if (learner.loadStateFile(fileName))
            initialized = true;
        else {
            printf("File read failed!!!\n");
            return 0;
        }
    }
    
    learner.writeStateFile();
    return 0;
    
    
    // initialize from scratch
    if (!initialized) {
        learner.appendGrid(4,81,0.5);
        learner.connectNearestNeighbors(16);
        //learner.appendReaches();
        //learner.initializeTransitionReward(1.0);
    }

    /*** DO INTRINSICALLY MOTIVATED MODEL LEARNING ***/
    
    State* s = NULL;
    TransitionAction* a = NULL;
    
    int count = 0;
    while (true)
    {
        if ( !a || !a->isRunning() )
        {
            printf("\nCOUNT: %d\n",count);
            printf("least tried: %d\n\n",learner.leastTriedTransition()->getTimesTried());
            
            s = learner.getDiscreteState();
            if (!s) break;
            
            a = s->greedyTransition();
            if (!a) break;
            
            a->start(&count);
        }
        
        printf(".");
        yarp::os::Time::delay(0.2);
    }
 
    printf("All finished\n");
    return 1;
}


/*if ( createReachTargets ) {
 // create a grid of reach targets in task space
 std::vector<Point_3> reachTargets = tableSample( -0.4, -0.1, 0.0, 0.4, 0.0, 0.05 );
 printf("made %d reach targets\n",reachTargets.size());
 //Point_3 p(-0.3,0.1,0.0);
 //Point_3 q(-0.2,0.2,0.1);
 }*/