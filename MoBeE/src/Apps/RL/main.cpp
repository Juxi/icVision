#include <unistd.h>
#include <time.h>
#include "learner.h"

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
    while ( count < 100) {
        
        printf("\n*****************************************************************************\n\n");
        
        Learner::State* ts = torso_learner.getDiscreteState();
        Learner::State* ls = left_arm_learner.getDiscreteState();
        Learner::State* rs = right_arm_learner.getDiscreteState();
        
        std::list<Learner::State::TransitionAction*>::iterator p = ts->transitionActions.begin();
        std::list<Learner::State::TransitionAction*>::iterator q = ls->transitionActions.begin();
        std::list<Learner::State::TransitionAction*>::iterator r = rs->transitionActions.begin();
        int j=0,
            tIdx = rand() % (ts->transitionActions.size()),
            lIdx = rand() % (ls->transitionActions.size()),
            rIdx = rand() % (rs->transitionActions.size());
        while (j < tIdx) { p++; j++; } j=0;
        while (j < lIdx) { q++; j++; } j=0;
        while (j < rIdx) { r++; j++; }
        
        (*p)->start();
        (*q)->start();
        (*r)->start();
        
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
