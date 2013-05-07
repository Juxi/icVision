#include <unistd.h>
#include <time.h>
#include "learner.h"
#include "actionReach.h"
#include "actionTransition.h"

std::vector<Point_d> gridSample( int dim, int num, double scaling )
{
    // Sample num grid points from dim dimensional space in the cube between min and max
    std::vector<Point_d> points;
    points.reserve(num);
    
    
    CGAL::points_on_cube_grid_d( dim, scaling*0.5, (std::size_t)num, std::back_inserter(points), Creator_d(dim));
    
    std::cout << "Using Samples: \n" <<std::endl;
    
    // shift the sample set to the interval [0,1]
    std::vector<double> v;
    for (int i=0; i<dim; i++) v.push_back(0.5);
    Vector_d shift(dim,v.begin(),v.end());
    for ( std::vector<Point_d>::iterator i=points.begin(); i!=points.end(); ++i ){
        *i += shift;
        std::cout << *i << std::endl;
    }
    
    return points;
}

struct StateDst {
    State* s;
    bool operator() (State* i,State* j) { return (*s-*i).squared_length() < (*s-*j).squared_length(); }
    StateDst(State* state):s(state){}
};

int main(int argc, char *argv[])
{
    yarp::os::Property config;
    config.fromCommand(argc,argv);
    
    // prepare the random number generator
    srand(time(0));
    
    // instantiate reinforcement learner
    Learner learner(16,"icubSim","right_arm",true);
    
    int count = 0;
    if ( config.check("count") ) count = config.find("count").asInt();
    
    if ( config.check("file") ) {
        // load the learner state from file
        std::string fileName = config.find("file").asString().c_str();
        learner.loadFile(fileName);
    } else {
        // append a grid of states
        std::vector<Point_d> samples = gridSample(4,81,0.5);
        //std::vector<Point_d> samples = gridSample(2,4,0.5);
        
        std::vector< yarp::os::ConstString > markers = learner.getMarkers();
        for ( std::vector<Point_d>::iterator i = samples.begin(); i!=samples.end(); ++i ) {
            State* s = learner.appendState(*i);
            printf("appended state: %p\n",s);
        }
        // connect all the states to all the other states and add reach actions
        printf("\nConnecting States...\n\n");
        std::vector<State*> states = learner.getStates();
        std::vector<State*> nearestStates = states;
        
        //for ( std::vector<State*>::iterator i = states.begin(); i!=states.end(); ++i )
        //    printf("states: %p\n",*i);
        //for ( std::vector<State*>::iterator i = nearestStates.begin(); i!=nearestStates.end(); ++i )
        //    printf("nearest_states: %p\n",*i);
        
        
        //std::sort (nearestStates.begin(), nearestStates.end(), StateDst(*states.begin()));
        //for ( std::vector<State*>::iterator i = nearestStates.begin(); i!=nearestStates.end(); ++i )
        //    printf("sorted_nearest_states: %p %f\n",*i,(**i-**states.begin()).squared_length());
        
        //printf("------------------------------------\n");
        
        for ( std::vector<State*>::iterator i = states.begin(); i!=states.end(); ++i )
        {
            printf("state: %p\n",*i);
            
            std::sort (nearestStates.begin(), nearestStates.end(), StateDst(*i));
            
            for ( std::vector<State*>::iterator j = nearestStates.begin(); j!=nearestStates.end(); ++j )
                printf("sorted_nearest_states: %p %f\n",*j,(**j-**i).squared_length());
                
            int num = 0;
            for ( std::vector<State*>::iterator j = nearestStates.begin(); j!=nearestStates.end() && num<16; ++j ) {
                if (*i!=*j) {
                    printf("  connecting %p, %p\n",*i,*j);
                    learner.appendTransitionAction(*i, *j);
                    num++;
                }
            }
            
            for ( std::vector<yarp::os::ConstString>::iterator j=markers.begin(); j!=markers.end(); ++j)
                learner.appendReachAction(*i, *j);
        }
    }
    
    Point_3 p(-0.3,0.1,0.0);
    Point_3 q(-0.2,0.2,0.1);
    Point_3* targetPoint = &p;
    yarp::os::ConstString reachTarget;
    reachTarget = learner.mkSphere(targetPoint->x(), targetPoint->y(), targetPoint->z(), 0.03);
    
    
    learner.tryReaches(p);
    //learner.tryStateTransitions(1000);
    
 
    printf("\n\nRight Arm Learner:\n");
    learner.print(true);
    
    
    
    
    
    printf("All finished\n");
    return 1;
}
