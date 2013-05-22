#include <unistd.h>
#include <time.h>
#include "learner.h"
#include "actionReach.h"
#include "actionTransition.h"

std::vector<Point_3> tableSample( double xMin, double xMax, double yMin, double yMax, double z, double delta )
{
    // table height: z = -0.075
    std::vector<Point_3> result;
    for ( double x = xMin; x<=xMax; x+=delta ) {
        for ( double y = yMin; y<=yMax; y+=delta ) {
            result.push_back(Point_3(x,y,z));
        }
    }
    return result;
}

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
    bool connectToMoBeE     = true;
    bool appendReaches      = false;
    bool createReachTargets = false;
    
    yarp::os::Property config;
    config.fromCommand(argc,argv);
    
    // prepare the random number generator
    srand(time(0));
    
    // instantiate reinforcement learner
    Learner learner(16,"icubSim","right_arm",connectToMoBeE);
    
    // load the learner state from file
    bool readFile = false;
    if ( config.check("file") ) {
        std::string fileName = config.find("file").asString().c_str();
        if (!learner.loadStateFile(fileName)) {
            printf("File read failed!!!\n");
            return 0;
        } else readFile = true;
    }
    
    // start the counter from a number > 0
    //int count = 0;
    //if ( config.check("count") )
    //    count = config.find("count").asInt();
    
    // create a grid of states
    if (!readFile) {
        std::vector<Point_d> samples = gridSample(4,81,0.5);
            //std::vector<Point_d> samples = gridSample(2,4,0.5);
        std::vector< yarp::os::ConstString > markers = learner.getMarkers();
        for ( std::vector<Point_d>::iterator i = samples.begin(); i!=samples.end(); ++i ) {
            State* s = learner.appendState(*i,0);
            //printf("appended state: %p\n",s);
        }
        
        // connect all the states to n nearest neighbors
        int n = 16;
        printf("\nConnecting States...\n\n");
        std::vector<State*> states = learner.getStates();
        std::vector<State*> nearestStates = states;
        for ( std::vector<State*>::iterator i = states.begin(); i!=states.end(); ++i ) {
                //printf("state: %p\n",*i);
            std::sort (nearestStates.begin(), nearestStates.end(), StateDst(*i));
                /*for ( std::vector<State*>::iterator j = nearestStates.begin(); j!=nearestStates.end(); ++j )
                 printf("sorted_nearest_states: %p %f\n",*j,(**j-**i).squared_length());
                 */  
            int m = 0;
            for ( std::vector<State*>::iterator j = nearestStates.begin(); j!=nearestStates.end() && m<n; ++j ) {
                if (*i!=*j) {
                    //printf("  connecting %p, %p\n",*i,*j);
                    learner.appendTransitionAction(*i, *j);
                    m++;
                }
            }
            if ( appendReaches ) {
                for ( std::vector<yarp::os::ConstString>::iterator j=markers.begin(); j!=markers.end(); ++j)
                    learner.appendReachAction(*i, *j);
            }
        }
        
        learner.initializeReward(1.0);
    }
    
    if ( createReachTargets ) {
        // create a grid of reach targets in task space
        std::vector<Point_3> reachTargets = tableSample( -0.4, -0.1, 0.0, 0.4, 0.0, 0.05 );
        printf("made %d reach targets\n",reachTargets.size());
    }
    
    //Point_3 p(-0.3,0.1,0.0);
    //Point_3 q(-0.2,0.2,0.1);
    //learner.print();
    //learner.writeStateFile();
    
    learner.learnModel_IM();
 

    printf("All finished\n");
    return 1;
}
