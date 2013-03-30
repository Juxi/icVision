#include <unistd.h>
#include <time.h>
#include "learner.h"

Learner::State::Action* takeRandomAction(Learner& learner)
{
    Learner::State* s = learner.getDiscreteState();
    printf("CURRENT STATE: %p\n",s);
    
    //int transitionIdx,reachIdx;
    if ( rand()%4 < 1 )
    {
        // Try a random state transition
        //int idx = rand() % s->transitionActions.size();
        //printf("trying action %d\n",idx);
        
        
        std::list<Learner::State::TransitionAction*>::iterator a = s->transitionActions.begin();
        Learner::State::TransitionAction* leastTriedAction = *s->transitionActions.begin();
        for ( a = s->transitionActions.begin(); a != s->transitionActions.end(); ++a ) //a++;
        {
            std::pair<const Learner::State*,double> belief = (*a)->belief();
            printf("  action: %p, %d tries, destination: %p it leads to state: %p with prob. %f\n",*a,(*a)->isTried(),(*a)->destination(),belief.first,belief.second);
            if ( (*a)->isTried() < leastTriedAction->isTried() )
                leastTriedAction = *a;
        } 
        
        
        printf("RUNNING LEAST TRIED STATE TRANSITION: %p\n",leastTriedAction);
        leastTriedAction->start();
        
        return leastTriedAction;
    }
    else
    {   // Try a random reach
        //printf("Reach Actions: %d\n", s->reachActions.size());
        
        printf("RUNNING AN EASY REACH\n");
        if ( s->reachActions.size() > 0 ) {
            std::list<Learner::State::ReachAction*>::iterator a = s->reachActions.begin();
            (*a)->runReach((*a)->easyReach());
            return *a;
        }
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
    for (int i=0; i<5; i++)
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
        //printf("\n*****************************************************************************\n\n");
        
        //takeRandomAction(torso_learner);
        //takeRandomAction(left_arm_learner);
        
        
        printf("\nMain: COUNT = %d\n",count);
        Learner::State::Action* a = takeRandomAction(right_arm_learner);
        while ( a->isRunning() ) {yarp::os::Time::delay(1.0);}
        
        
        
        
        
        count++;
    }
    
    torso_learner.mutex.wait();
    right_arm_learner.mutex.wait();
    left_arm_learner.mutex.wait();
    
    //printf("Torso Learner:\n");     torso_learner.print(true);
    printf("Right Arm Learner:\n"); right_arm_learner.print(true);
    //printf("Left Arm Learner:\n");  left_arm_learner.print(true);
    
    
    printf("All finished\n");
    return 1;
}
