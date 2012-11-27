#include <unistd.h>
#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <yarp/sig/all.h>
#include <iostream.h>
#include <sstream>

int main(int argc, char *argv[])
{
    // prepare yarp
    yarp::os::Network net;
    
    // prepare the random number generator
    srand(time(0));
    
    // estimate limits of the reachable workspace
    double x_min = -0.5,
            x_max = 0.0,
            y_min = -0.5,
            y_max = 0.5,
            z_min = 0.0,
            z_max = 0.5;
    
    // sample the reachable workspace
    double p[100][3];
    for (int i=0; i<100; i++){
        p[i][0] = x_min + (double)rand()/RAND_MAX * (x_max-x_min);
        p[i][1] = y_min + (double)rand()/RAND_MAX * (y_max-y_min);
        p[i][2] = z_min + (double)rand()/RAND_MAX * (z_max-z_min);
    }
    
    // connect to the world model RPC port
    yarp::os::RpcClient world;
    world.open("/world_port_cmd");
    net.connect("/world_port_cmd","/MoBeE/world");
    
    // display the samples in the MoBeE world model
    for (int i=0; i<100; i++)
    {
        // add a sphere to mark the target point
        yarp::os::Bottle cmd, rsp;
        cmd.addVocab(yarp::os::Vocab::encode("mk"));
        cmd.addVocab(yarp::os::Vocab::encode("sph"));
        cmd.addDouble(0.01);
        cmd.addDouble(p[i][0]);
        cmd.addDouble(p[i][1]);
        cmd.addDouble(p[i][2]);
        printf("command: %s\n", cmd.toString().c_str());
        world.write(cmd,rsp);
        printf("  response: %s\n", rsp.toString().c_str());
        yarp::os::ConstString objectName = rsp.get(0).asString();
        cmd.clear();
        rsp.clear();
        
        // make it a 'target' so it doesn't cause collisions with stuff
        cmd.addVocab(yarp::os::Vocab::encode("def"));
        cmd.addString(objectName);
        cmd.addString("tgt");
        printf("command: %s\n", cmd.toString().c_str());
        world.write(cmd,rsp);
        printf("  response: %s\n", rsp.toString().c_str());
        cmd.clear();
        rsp.clear();
        
        printf("\n");
    }
    
    // reach to each target point
    for (int i=0; i<100; i++)
    {
        
    }
	
	return 1;
}
