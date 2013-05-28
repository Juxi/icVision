#include <unistd.h>
#include <time.h>
#include "learner.h"
#include "util.h"
#include "rl_problem.h"

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
    if (!initialized) {
        printf("ERROR: Need a model to begin learning reaches! Use --file 'filename'\n");
        return 0;
    }
    
    learner.setStateFileName("arm_state.dat");      // contains internal learner state
    learner.setHistoryFileName("arm_history.dat");  // history of states visited, actions taken, and rewards received
    learner.setModelLearning(false);                // don't mess with the state transition probabilities
    
    /* INITIALIZE REACH ACTIONS */
    //learner.appendReaches();
    //learner.initializeTransitionReward(0.0);
    //learner.initializeReachReward(1.0);
    //learner.valueIteration();
    //learner.writeStateFile();
    //return 1;
    
    // reach targets in task space
    RL_Problem_Set problems(&learner);
    problems.sampleInit( -0.4, -0.1, 0.0, 0.4, 0.0, 0.0, 0.05 );
    
    State* s = NULL;
    Action* a = NULL;
    
    int targetCount = 0;    // counts reach targets
    int numTries;
    
    for ( RL_Problem_Set::iterator i=problems.begin(); i!=problems.end(); ++i)
    {
        RL_Problem* prob = *i;
        // choose an RL problem
        //problems.evaluateInterest();
        //RL_Problem* prob = problems.mostInteresting();
        
        printf("\n\n\nNew Reach Target: (%f %f %f)\n\n",prob->target.x(), prob->target.y(), prob->target.z());
        yarp::os::ConstString sph = learner.mobee.mkSphere(prob->target.x(), prob->target.y(), prob->target.z(), 0.02);
        
        //learner.predictRewards(prob->target);
        
        //learner.randomTransitions(5); // if we don't do this, must run value iteration explicitly
        //printf("AHHHHHHHHHHHHHHHHHHHHHHHHHHHHH\n");
        
        learner.initializeReachReward(1.0);
        
        int count = prob->numActions;               // counts actions executed
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
    } //while ( problems.leastTried() < 10 );
    
    printf("All finished\n");
    return 1;
}