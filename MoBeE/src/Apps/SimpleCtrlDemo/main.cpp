#include "partController.h"
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[])
{
    
    srand(time(0));

	PartController c("icubSim","right_arm",20);
    c.start();
    
    while (true)
    {
        yarp::os::Bottle position;
        yarp::os::Bottle force;
        
        for (int i=0; i<16; i++) {
            position.addDouble((double)rand()/RAND_MAX);
            force.addDouble((double)rand()/RAND_MAX);
        }
        
        c.setAttractorPosition(&position);
        c.setConstForce(&force);
        
        printf("Position: %s\n",position.toString().c_str());
        printf("Force: %s\n",force.toString().c_str());
        
        sleep(5);
    }
    
    

    printf("All finished\n");
    return 1;
}
