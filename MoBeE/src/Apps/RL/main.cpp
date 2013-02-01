#include <unistd.h>
#include <time.h>
#include "learner.h"

int main(int argc, char *argv[])
{
    // prepare the random number generator
    srand(time(0));
    
    // instantiate a reinforcement learner for the torso, checking for state transition at 5Hz (200ms period)
    Learner learner("icubSim","torso",200);

    // pick some random states (poses) and add them to the state space of the learner
    Point_d q(3,CGAL::ORIGIN);
    for (int i=0; i<6; i++){
        std::list<double> thisState;
        for (int j=0; j<3; j++) thisState.push_back((double)rand()/RAND_MAX);
        Point_d q(thisState.size(),thisState.begin(),thisState.end());
        learner.appendState(q);
    }
    //learner.print();
    
    // send control commands and keep track of the current state as the robot moves
    learner.start();
    
    // wait for the robot to relax into the attractor for the current state
    sleep(10);
    
    // take random actions... the learner will learn an MDP for the bodypart
    while (true) {
        learner.takeRandomAction();
        //learner.print();
    }
    
    learner.stop();
    
    
    //printf("------------------------------------------------------------------------------\n");
    //std::vector<const Learner::State*> states = learner.getStates();
    //learner.deleteState(states.at(3));
    //learner.print();
    
    printf("All finished\n");
    return 1;
}
