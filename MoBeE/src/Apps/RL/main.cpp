#include "learner.h"

int main(int argc, char *argv[])
{
    Learner learner;
    
    Point_d q(3,CGAL::ORIGIN);
    
    for (int i=0; i<4; i++){
        learner.appendFullyConnectedState(q);
    }
    
    learner.print();
    
    printf("All finished\n");
    return 1;
}
