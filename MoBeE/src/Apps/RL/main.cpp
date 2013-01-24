#include <unistd.h>
#include <time.h>
#include "learner.h"

int main(int argc, char *argv[])
{
    // prepare the random number generator
    srand(time(0));
    
    Learner learner("icubSim","right_arm",200);

    
    // make some random states
    Point_d q(3,CGAL::ORIGIN);
    for (int i=0; i<6; i++){
        std::list<double> thisState;
        for (int j=0; j<16; j++)
            thisState.push_back((double)rand()/RAND_MAX);
        Point_d q(thisState.size(),thisState.begin(),thisState.end());
        learner.appendState(q);
    }
    learner.print();
    
    sleep(1);
    
    // start a thread to keep track of the current state as the robot moves
    learner.start();
    
    sleep(3);
    
    while (true) {
        learner.takeRandomAction();
        learner.print();
    }
    
    // choose an action from the current state
    
    // execute the action
    
    // wait for a state change or for the robot to stop moving
    
    // update state transition probabilities
    
    learner.stop();
    
    
    //printf("------------------------------------------------------------------------------\n");
    //std::vector<const Learner::State*> states = learner.getStates();
    //learner.deleteState(states.at(3));
    //learner.print();
    
    printf("All finished\n");
    return 1;
}
