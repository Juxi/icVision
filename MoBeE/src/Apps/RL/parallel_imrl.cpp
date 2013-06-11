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
    Learner right_arm(16,"icubSim","right_arm",connectToMoBeE);
    Learner left_arm(16,"icubSim","left_arm",connectToMoBeE);
    Learner torso(3,"icubSim","torso",connectToMoBeE);
    
    // initialize from file
    /*bool torso_initialized = false;
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
    }*/
    
    // initialize from scratch
    //if (!torso_initialized) {
        torso.appendGrid(3,27,0.5);
        torso.connectNearestNeighbors(8);
        //torso.initializeTransitionReward(1.0);
    //}
    //if (!arm_initialized) {
        right_arm.appendGrid(4,81,0.5);
        right_arm.connectNearestNeighbors(16);
        //right_arm.initializeTransitionReward(1.0);
    //}
        left_arm.appendGrid(4,81,0.5);
        left_arm.connectNearestNeighbors(16);
    
    // name the output files
    torso.setStateFileName("torso_state.dat");
    torso.setHistoryFileName("torso_history.dat");
    right_arm.setStateFileName("right_arm_state.dat");
    right_arm.setHistoryFileName("right_arm_history.dat");
    left_arm.setStateFileName("left_arm_state.dat");
    left_arm.setHistoryFileName("left_arm_history.dat");
    
    State   *ts = NULL, // torso state
            *ls = NULL, 
            *rs = NULL; // arm ststa
    
    Action  *ta = NULL, // torso action
            *la = NULL,
            *ra = NULL; // arm action
    
    int torso_count = 0;
    int left_arm_count = 0;
    int right_arm_count = 0;
    while ( true )
    {
        if ( (!ta || !ta->isRunning()) && (!la || !la->isRunning()) && (!ra || !ra->isRunning()) )
        {
            printf("\n*************************************************************\n\n");
            printf("TORSO_COUNT: %d\n",torso_count);
            printf("LEFT_ARM_COUNT: %d\n",left_arm_count);
            printf("RIGHT_ARM_COUNT: %d\n\n",right_arm_count);
            
            ts = torso.getDiscreteState(); if (!ts) break;
            ls = left_arm.getDiscreteState(); if (!ls) break;
            rs = right_arm.getDiscreteState(); if (!rs) break;
            
            ta = ts->eGreedyAction(0.1); if (!ta) break;
            la = ls->eGreedyAction(0.1); if (!la) break;
            ra = rs->eGreedyAction(0.1); if (!ra) break;
            
            ta->start(&torso_count);
            la->start(&left_arm_count);
            ra->start(&right_arm_count);
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