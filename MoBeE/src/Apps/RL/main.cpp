#include <unistd.h>
#include <time.h>
#include "learner.h"
#include "actionReach.h"

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
            learner.appendState(*i);
        }
        // connect all the states to all the other states and add reach actions
        std::vector<State*> states = learner.getStates();
        for ( std::vector<State*>::iterator i = states.begin(); i!=states.end(); ++i ) {
            for ( std::vector<State*>::iterator j = states.begin(); j!=states.end(); ++j ) {
                if ( *i != *j ) learner.appendTransitionAction(*i, *j);
            }
            for ( std::vector<yarp::os::ConstString>::iterator j=markers.begin(); j!=markers.end(); ++j) {
                learner.appendReachAction(*i, *j);
            }
        }
    }
    
    Point_3 p(-0.3,0.1,0.0);
    Point_3 q(-0.2,0.2,0.1);
    Point_3* targetPoint = &q;
    yarp::os::ConstString reachTarget;
    reachTarget = learner.mkSphere(targetPoint->x(), targetPoint->y(), targetPoint->z(), 0.02);
    
    // TAKE ACTIONS AND DO RL
    std::string outFileBaseName = "outFile";
    while ( count < 10000)
    {
        printf("\nMain: COUNT = %d\n",count);
        
        Action* a = NULL;
        //if ( rand()%2 ) {
        //    printf("Taking GREEDY Action.\n");
        //    a = learner.getDiscreteState()->greedyAction();
        //} else {
            if ( rand()%2 ) {
                printf("Taking EXPLORATORY Action.\n");
                a = learner.getDiscreteState()->exploreTransition();
            }
            else {
                printf("Taking REACH Action.\n");
                a = learner.getDiscreteState()->reach();
            }
        //}
        
        ReachAction* reach = dynamic_cast<ReachAction*>(a);
        //if ( reach ) learner.defTarget(reachTarget);
        
        a->start(*targetPoint);
        while ( a->isRunning() ) {yarp::os::Time::delay(1.0);}
        
        //if ( reach ) learner.defObstacle(reachTarget);
        
        learner.doRL();
        
        count++;
        // write an output file 
        if (count%10 == 0) {
            std::stringstream suffix;
            suffix << count << ".ini";
            std::string outFile = outFileBaseName + suffix.str();
            
            //learner.rmGeom(reachTarget);
            //if ( targetPoint == &p ) targetPoint = &q;
            //else if ( targetPoint == &q ) targetPoint = &p;
            //reachTarget = learner.mkSphere(targetPoint->x(), targetPoint->y(), targetPoint->z(), 0.02);
            //learner.generateValueFunction(*targetPoint);
            learner.writeFile(outFile);
        }
    }
 
    printf("\n\nRight Arm Learner:\n");
    learner.print(true);
    
    
    
    
    
    printf("All finished\n");
    return 1;
}
