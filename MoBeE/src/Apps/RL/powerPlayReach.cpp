#include <unistd.h>
#include <time.h>
#include "learner.h"
#include "rl_problem.h"
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
    } else {
        learner.appendGrid(4,25,0.75);
        learner.connectNearestNeighbors(8);
        learner.initializeTransitionProbs();
        learner.appendReaches();
    }
    
    learner.setStateFileName("arm_state.dat");      // contains internal learner state
    learner.setHistoryFileName("arm_history.dat");  // history of states visited, actions taken, and rewards received
    learner.setModelLearning(false);                // don't mess with the state transition probabilities
    learner.predictRewards(Point_3(0,0,0));
    learner.writeStateFile();
    
    // reach targets in task space
    RL_Problem_Set problems(&learner);
    problems.sampleInit( -0.4, -0.1, 0.0, 0.4, 0.0, 0.4, 0.1 );
    
    State* s = NULL;
    Action* a = NULL;
    
    int targetCount = 0;    // counts reach targets
    int numTries;
      
    do {
        // choose an RL problem
        //problems.evaluateInterest();
        RL_Problem* prob = problems.leastTriedProblem();
        
        printf("\n\n\nNew Reach Target: (%f %f %f)\n\n",prob->target.x(), prob->target.y(), prob->target.z());
        yarp::os::ConstString sph = learner.mobee.mkSphere(prob->target.x(), prob->target.y(), prob->target.z(), 0.02);
        
        learner.predictRewards(prob->target);
        
        //learner.randomTransitions(5); // if we don't do this, must run value iteration explicitly
        //printf("AHHHHHHHHHHHHHHHHHHHHHHHHHHHHH\n");
        
        int count = prob->numActions; // counts actions executed
        while ( prob->numActions - count < 10 )
        {
            if ( !a || !a->isRunning() )
            {
                printf("\nTARGET: %d, ACTION_COUNT: %d\n\n",targetCount,prob->numActions);
                s = learner.getDiscreteState(); if (!s) break;
                a = s->eGreedyAction(0.5); if (!a) break;
                a->start( prob->target, &prob->numActions );
            }
            
            printf(".");
            yarp::os::Time::delay(0.2);
        }
        //yarp::os::Time::delay(0.2);
        learner.mobee.rmGeom(sph);
        targetCount++;
    } while ( problems.leastTried() < 10 );
 
    printf("All finished\n");
    return 1;
}