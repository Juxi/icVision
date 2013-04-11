#include <unistd.h>
#include <time.h>
#include "learner.h"

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
    
    // path to the XML file that defines the robot model
    std::string fileName = config.find("file").asString().c_str();
    
    printf("WTF!?!\n\n");
    
    // prepare the random number generator
    srand(time(0));
    
    // instantiate reinforcement learner
    Learner learner(16,"icubSim","right_arm",false);
    
    learner.loadFile(fileName);
    
    learner.print();
    //std::string outFileName = "testFile.ini";
    //learner.writeFile(outFileName);
    
    /*
    
    std::vector<Point_d> samples = gridSample(4,33,0.5);
    //for ( std::vector<Point_d>::iterator i = samples.begin(); i!=samples.end(); ++i )
    //    learner.appendState(*i);
    
    int count = 0;
    while ( count < 500)
    {
        printf("\nMain: COUNT = %d\n",count);
        
        //learner.getDiscreteState()->takeRandomAction();
        learner.doRL();
        
        //printf("Waiting for Action to complete.\n");
        //printf("Action returned reward: %f\n",a->checkReward());
        //while ( a->isRunning() ) {yarp::os::Time::delay(1.0);}
        count++;
    }
 
    printf("Right Arm Learner:\n");
    //learner.print(true);
    */
    
    printf("All finished\n");
    return 1;
}
