#include "partController.h"
#include <unistd.h>

int main(int argc, char *argv[])
{	
	PartController controller("icubSim","torso",20);
	controller.start();
	
	yarp::os::Network net;
	yarp::os::BufferedPort<yarp::os::Bottle> port;
	port.open("/commandOut");
	net.connect("/commandOut","/partController");
	
	yarp::os::Bottle a,b;
	a.add(10.0);
	a.add(0.0);
	a.add(0.0);
	b.add(-10.0);
	b.add(0.0);
	b.add(0.0);
	
	int i = 0;
	while (true) {
	
		yarp::os::Bottle& msg = port.prepare();
		msg.clear();
		msg.add("x");
		
		if ( i % 2 == 0 )	msg.addList() = a;
		else				msg.addList() = b;
		
		printf("\nSending command to part controller: %s\n", msg.toString().c_str());
		port.write();
		//msg.clear();
		
		i++;
		
		
		sleep(2);
	}
	
	return 1;	
}
