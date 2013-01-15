#include "learner.h"

int main(int argc, char *argv[])
{
    Learner learner(20);
    
    // make some random states
    Point_d q(3,CGAL::ORIGIN);
    for (int i=0; i<6; i++){
        learner.appendFullyConnectedState(q);
    }
    learner.print();
    
    // start a thread to keep track of the current state as the robot moves
    
    // choose an action from the current state
    
    // execute the action
    
    // wait for a state change or for the robot to stop moving
    
    // update state transition probabilities
    
    
    //printf("------------------------------------------------------------------------------\n");
    //std::vector<const Learner::State*> states = learner.getStates();
    //learner.deleteState(states.at(3));
    //learner.print();
    
    printf("All finished\n");
    return 1;
}
