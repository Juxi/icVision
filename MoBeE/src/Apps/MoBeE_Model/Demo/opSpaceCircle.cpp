/*******************************************************************
 ***               Copyright (C) 2012 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ***                                                             ***
 ***  This is a demonstration of using feed forward operational  ***
 ***  space control within the MoBeE framework to cause the hand ***
 ***  to oscillate.                                              ***
 ******************************************************************/
#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <yarp/sig/all.h>
#include <unistd.h>

class Oscillator {                                          // a simple, reusable oscillator
public:
    Oscillator() : min(-1), max(1), freq(1), phase(0) {}    // the default constructor
    double evaluate( double t ) {                           // a function to sample the oscillator
        double amplitude = (max-min)/2;
        double mean = min+amplitude;
        return mean + amplitude*sin(freq*t+phase*M_PI/180);
    }
    double min, max, freq, phase;                           // the necessary parameters
};

int main(int argc, char *argv[])
{
    // initialize the YARP network
	yarp::os::Network net;
    
    // a port for sending commands to MoBeE
	yarp::os::BufferedPort<yarp::os::Bottle> left_arm;
    left_arm.open("/circle_demo/left_arm_cmd:o");
    net.connect("/circle_demo/left_arm_cmd:o","/MoBeE/left_arm/cmd:i");
    
    // an oscillator for the y component of operational space control commands
    // note: this is the iCub's left/right direction
    Oscillator y;
    y.min = -50;
    y.max = 50;
    
    // an oscillator for the z component of operational space control commands
    // note: this is the iCub's up/down direction
    Oscillator z;
    z.min = -50;
    z.max = 50;
	z.phase = 90;
    
    // a counter of discrete time steps (to control sampling from the oscillators)
    int     i = 0;
    
    // a period for our control frequency (in several units for convenience)
    double  msPeriod = 50.0,            // milliseconds
            sPeriod = msPeriod/1000,    // seconds
            usPeriod = 1000*msPeriod,   // microseconds
            t;                          // the 'current' time (to sample the oscillators)
    
    // control the iCub
	while (true) {
	
        // compute the current time in seconds (since time 0.0)
        t = i*sPeriod;
        
        // increment the timestep
        i++;
        
        // get a bottle from YARP, in which to put our control command
        yarp::os::Bottle& msgR = left_arm.prepare();
        msgR.clear();
        
        // indicates an operational space control command
        msgR.addVocab(yarp::os::Vocab::encode("opsp"));
        
        // to be applied to the 'marker' called 'right_hand'
        msgR.addString("left_hand");
        
        // a bottle to represent a force vector
        yarp::os::Bottle cmd;
        cmd.addDouble(0.0);
        cmd.addDouble(100*y.evaluate(t));
        cmd.addDouble(100*z.evaluate(t));
		
        // nest the force vector inside our control command bottle
        msgR.addList() = cmd;
        
        // send the command to MoBeE
        left_arm.write();
        
        // inform the user
        printf("Sending Command: %s\n", msgR.toString().c_str());
	
        // take a break for a quick moment, so as not to flood the port with data
        usleep(usPeriod);
	}
	
	return 1;	
}
