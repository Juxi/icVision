#include <unistd.h>
#include <time.h>
#include "learner.h"

int main(int argc, char *argv[])
{
    // prepare the random number generator
    srand(time(0));
    
    // instantiate a reinforcement learner for the torso, checking for state transition at 5Hz (200ms period)
    Learner torso_learner("icubSim","torso",200);
    Learner right_arm_learner("icubSim","right_arm",200);
    Learner left_arm_learner("icubSim","left_arm",200);

    // pick some random states (poses) and add them to the state space of the learners
    for (int i=0; i<10; i++)
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
    //learner.print();
    
    // send control commands and keep track of the current state as the robot moves
    torso_learner.start();
    right_arm_learner.start();
    left_arm_learner.start();
    
    // wait for the robot to relax into the attractor for the current state
    sleep(10);
    
    // take random actions... the learner will learn an MDP for the bodypart
    while (true) {
        torso_learner.takeRandomAction();
        left_arm_learner.takeRandomAction();
        right_arm_learner.takeRandomAction();
        //left_arm_learner.randomReach("left_hand");
        //right_arm_learner.randomReach("right_hand");
        
        //learner.print();
    }
    
    torso_learner.stop();
    left_arm_learner.stop();
    right_arm_learner.stop();
    
    
    //printf("------------------------------------------------------------------------------\n");
    //std::vector<const Learner::State*> states = learner.getStates();
    //learner.deleteState(states.at(3));
    //learner.print();
    
    printf("All finished\n");
    return 1;
}
