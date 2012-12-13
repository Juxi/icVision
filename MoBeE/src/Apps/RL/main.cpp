#include "learner.h"

int main(int argc, char *argv[])
{
    Learner learner;
    
    Point_d q(3,CGAL::ORIGIN);
    
    for (int i=0; i<6; i++){
        learner.appendFullyConnectedState(q);
    }
    
    learner.print();
    
    printf("------------------------------------------------------------------------------\n");
    
    std::vector<const Learner::State*> states = learner.getStates();
    learner.deleteState(states.at(3));
    
    learner.print();
    
    printf("All finished\n");
    return 1;
}
