#include <unistd.h>
#include <time.h>
#include "learner.h"

bool takeRandomAction(Learner& learner)
{
    Learner::State* s = learner.getDiscreteState();
    if (1 /*rand()%2*/ )
    {   // Try a random state transition
        std::list<Learner::State::TransitionAction*>::iterator a = s->transitionActions.begin();
        int idx = rand()%(s->transitionActions.size());
        for ( int i=0; i<idx; i++ ) a++;
        (*a)->start();
    }
    else
    {   // Try a random reach
        std::list<Learner::State::ReachAction*>::iterator a = s->reachActions.begin();
        int idx = rand()%(s->reachActions.size());
        for ( int i=0; i<idx; i++ ) a++;
        
        // select a random point to reach to
        
        (*a)->start();
    }
    
    
}

int main(int argc, char *argv[])
{
    // prepare the random number generator
    srand(time(0));
    
    // instantiate reinforcement learners, checking for state transition at 5Hz (200ms period)
    Learner torso_learner("icubSim","torso",true);
    Learner right_arm_learner("icubSim","right_arm",true);
    Learner left_arm_learner("icubSim","left_arm",true);

    // pick some random states (poses) and add them to the state space of the learners
    for (int i=0; i<4; i++)
    {
        std::list<double> tState;
        for (int j=0; j<3; j++) tState.push_back((double)rand()/RAND_MAX);
        
        std::list<double> lState,rState;
        for (int j=0; j<16; j++) {
            lState.push_back((double)rand()/RAND_MAX);
            rState.push_back((double)rand()/RAND_MAX);
        }
        
        Point_d t(tState.size(),tState.begin(),tState.end());
        Point_d l(lState.size(),lState.begin(),lState.end());
        Point_d r(rState.size(),rState.begin(),rState.end());
        
        torso_learner.appendState(t);
        right_arm_learner.appendState(l);
        left_arm_learner.appendState(r);
    }
    
    int count = 0;
    while ( count < 100)
    {
        printf("\n*****************************************************************************\n\n");
        
        takeRandomAction(torso_learner);
        takeRandomAction(left_arm_learner);
        takeRandomAction(right_arm_learner);
        
        count++;
        yarp::os::Time::delay(1);
    }
    
    torso_learner.mutex.wait();
    right_arm_learner.mutex.wait();
    left_arm_learner.mutex.wait();
    
    printf("Torso Learner:\n");     torso_learner.print(true);
    printf("Right Arm Learner:\n"); right_arm_learner.print(true);
    printf("Left Arm Learner:\n");  left_arm_learner.print(true);
    
    
    printf("All finished\n");
    return 1;
}
