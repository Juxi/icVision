#include <unistd.h>
#include <time.h>
#include "learner.h"
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
    if (!initialized) {
        printf("ERROR: Need a model to begin learning reaches! Use --file 'filename'\n");
        return 0;
    }
    
    /* INITIALIZE REACH ACTIONS */
    //learner.appendReaches();
    //learner.initializeTransitionReward(0.0);
    //learner.initializeReachReward(1.0);
    //learner.writeStateFile();
    //return 1;
    
    // create a grid of reach targets in task space
    std::vector<Point_3> reachTargets = tableSample( -0.4, -0.1, 0.0, 0.4, 0.0, 0.05 );
    printf("\nMade %d reach targets\n",reachTargets.size());
    
    learner.setStateFileName("arm_state.dat");
    learner.setHistoryFileName("arm_history.dat");
    
    // don't mess with the state transition probabilities
    learner.setModelLearning(false);
    
    State* s = NULL;
    Action* a = NULL;
    
    int count = 0;          // counts actions executed
    int targetCount = 0;    // counts reach targets 
    for (std::vector<Point_3>::iterator i=reachTargets.begin(); i!=reachTargets.end(); ++i)
    {
        printf("\n\n\nNew Reach Target: (%f %f %f)\n\n",i->x(), i->y(), i->z());
        yarp::os::ConstString sph = learner.mobee.mkSphere(i->x(), i->y(), i->z(), 0.02);
        
        learner.initializeTransitionReward(0.0);
        learner.initializeReachReward(1.0);
        
        int leastTried = 0;
        while (leastTried <= targetCount)
        {
            if ( !a || !a->isRunning() )
            {
                printf("\nTARGET: %d, ACTION_COUNT: %d\n\n",targetCount,count);
                leastTried = learner.leastTriedReach()->getTimesTried();
                s = learner.getDiscreteState(); if (!s) break;
                a = s->greedyAction(); if (!a) break;
                a->start(*i,&count);
            }
            
            printf(".");
            yarp::os::Time::delay(0.2);
        }
        //yarp::os::Time::delay(0.2);
        learner.mobee.rmGeom(sph);
        targetCount++;
    }
 
    printf("All finished\n");
    return 1;
}