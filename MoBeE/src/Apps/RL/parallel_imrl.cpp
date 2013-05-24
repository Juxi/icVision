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
    Learner arm(16,"icubSim","right_arm",connectToMoBeE);
    Learner torso(3,"icubSim","torso",connectToMoBeE);
    
    // initialize from file
    bool torso_initialized = false;
    if ( config.check("torsofile") ) {
        std::string fileName = config.find("torsofile").asString().c_str();
        if (torso.loadStateFile(fileName))
            torso_initialized = true;
        else {
            printf("File read failed!!!\n");
            return 0;
        }
    }
    bool arm_initialized = false;
    if ( config.check("armfile") ) {
        std::string fileName = config.find("armfile").asString().c_str();
        if (arm.loadStateFile(fileName))
            arm_initialized = true;
        else {
            printf("File read failed!!!\n");
            return 0;
        }
    }
    
    // initialize from scratch
    if (!torso_initialized) {
        torso.appendGrid(3,27,0.5);
        torso.connectNearestNeighbors(6);
        torso.initializeReward(1.0);
    }
    if (!arm_initialized) {
        arm.appendGrid(4,81,0.5);
        arm.connectNearestNeighbors(16);
        arm.initializeReward(1.0);
    }
    
    // name the output files
    torso.setStateFileName("torso_state.dat");
    torso.setHistoryFileName("torso_history.dat");
    arm.setStateFileName("arm_state.dat");
    arm.setHistoryFileName("arm_history.dat");
    
    State   *ts = NULL, // torso state
            *as = NULL; // arm ststa
    
    TransitionAction    *ta = NULL, // torso action
                        *aa = NULL; // arm action
    
    int torso_count = 0;
    int arm_count = 0;
    while ( torso_count < 10000 || arm_count < 10000 )
    {
        if ( (!ta || !ta->isRunning()) && (!aa || !aa->isRunning()) )
        {
            printf("*************************************************************\n");
            printf("TORSO_COUNT: %d\n",torso_count);
            printf("ARM_COUNT: %d\n\n",arm_count);
            
            ts = torso.getDiscreteState(); if (!ts) break;
            as = arm.getDiscreteState(); if (!as) break;
            
            ta = ts->greedyTransition(); if (!ta) break;
            aa = as->greedyTransition(); if (!aa) break;
            
            ta->start(&torso_count);
            aa->start(&arm_count);
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