#include <unistd.h>
#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <yarp/sig/all.h>
#include <iostream.h>

class Oscillator
{
public:
    
    double min, max, freq, phase;
    Oscillator() : min(-1), max(1), freq(1), phase(0) {}
    
    double evaluate( double t ) {
        double amplitude = (max-min)/2;
        double mean = min+amplitude;
        return mean + amplitude*sin(freq*t+phase*M_PI/180);
    }
};

int main(int argc, char *argv[])
{	
	yarp::os::Network net;
	yarp::os::BufferedPort<yarp::os::Bottle> right_arm;
    right_arm.open("/right_arm_cmd");
    net.connect("/right_arm_cmd","/MoBeE/right_arm/");
    
    Oscillator y;
    y.min = -50;
    y.max = 50;
    
    Oscillator z;
    z.min = -50;
    z.max = 50;
	z.phase = 90;
    
    int     i = 0;
    double  msPeriod = 50.0,
            sPeriod = msPeriod/1000,
            usPeriod = 1000*msPeriod,
            t;
    
	while (true) {
	
        t = i*sPeriod;
        i++;
        
        yarp::os::Bottle& msgR = right_arm.prepare();

        msgR.clear();
        msgR.addVocab(yarp::os::Vocab::encode("opsp"));
        msgR.addString("right_hand");
        
        yarp::os::Bottle cmd;
        cmd.addDouble(0.0);
        cmd.addDouble(100*y.evaluate(t));
        cmd.addDouble(100*z.evaluate(t));
		
        msgR.addList() = cmd;
        right_arm.write();
        
        std::cout << "Sending Command: " << msgR.toString().c_str() << std::endl;
	
        usleep(usPeriod);
	}
	
	return 1;	
}
