/*******************************************************************
 ***               Copyright (C) 2012 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ***                                                             ***
 ***  This is a demonstration of using feed forward position     ***
 ***  control in the joint space within the MoBeE framework.     ***
 ******************************************************************/
#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <yarp/sig/all.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[])
{
    // prepare the YARP network
	yarp::os::Network net;
    
    // prepare the random number generator
    srand(time(0));
    
    // prepare 3 ports to talk to the iCub's torso and both arms
	yarp::os::BufferedPort<yarp::os::Bottle> torso, right_arm, left_arm;
    
    // fire up the threads
	torso.open("/poses_demo/torso_cmd:o");
    right_arm.open("/poses_demo/right_arm_cmd:o");
    left_arm.open("/poses_demo/left_arm_cmd:o");
    
    // connect the ports to the MoBeE model
    net.connect("/poses_demo/torso_cmd:o","/MoBeE/torso/cmd:i");
    net.connect("/poses_demo/right_arm_cmd:o","/MoBeE/right_arm/cmd:i");
    net.connect("/poses_demo/left_arm_cmd:o","/MoBeE/left_arm/cmd:i");
	
    // control the robot through an arbitrary number of poses
	int i = 0;
	while (i<100) {
	
        yarp::os::Bottle& tBottle = torso.prepare();    // get a bottle for our control command
        tBottle.clear();                                // make sure it's empty
        tBottle.addVocab(yarp::os::Vocab::encode("qatt"));   // prefix stands for 'q attractor', an attractor in joint space
        yarp::os::Bottle tCmd;                          // another bottle to hold the vector of joint positions
        for (int i=0; i<3; i++)                         // fill the vector with random normalized positions
            tCmd.addDouble((double)rand()/RAND_MAX);
        tBottle.addList() = tCmd;                       // nest the vector into the torso command 
        torso.write();                                  // send it off to MoBeE
        
        // repeat the above for the right arm
        yarp::os::Bottle& rBottle = right_arm.prepare();
        rBottle.clear();
        rBottle.addVocab(yarp::os::Vocab::encode("qatt"));
        yarp::os::Bottle rCmd;
        for (int i=0; i<16; i++)
            rCmd.addDouble((double)rand()/RAND_MAX);
        rBottle.addList() = rCmd;
        right_arm.write();
        
        // and again for the left arm
        yarp::os::Bottle& lBottle = left_arm.prepare();
        lBottle.clear();
        lBottle.addVocab(yarp::os::Vocab::encode("qatt"));
        yarp::os::Bottle lCmd;
        for (int i=0; i<16; i++)
            lCmd.addDouble((double)rand()/RAND_MAX);
        lBottle.addList() = lCmd;
        left_arm.write();
        
        // inform the user
        printf("Torso: %s\n",tBottle.toString().c_str());
        printf("Right: %s\n",rBottle.toString().c_str());
        printf("Left:  %s\n",lBottle.toString().c_str());
        printf("\n");
        
        // wait an arbitrary time for the motion to happen
        sleep(5);
        
        // increment the counter
        i++;
	}
	
	return 1;	
}
