#include <unistd.h>
#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <yarp/sig/all.h>
#include <iostream.h>

int main(int argc, char *argv[])
{	
	yarp::os::Network net;
	yarp::os::BufferedPort<yarp::os::Bottle> torso,
                                            left_arm,
                                            right_arm;
	torso.open("/torso_cmd");           net.connect("/torso_cmd","/MoBeE/torso/");
    right_arm.open("/right_arm_cmd");   net.connect("/right_arm_cmd","/MoBeE/right_arm/");
    left_arm.open("/left_arm_cmd");     net.connect("/left_arm_cmd","/MoBeE/left_arm/");
	
	yarp::os::Bottle torsoA,torsoB;
	torsoA.add(10.0);   torsoA.add(0.0);    torsoA.add(0.0);
	torsoB.add(-10.0);  torsoB.add(0.0);    torsoB.add(0.0);
	
    yarp::os::Bottle leftA,leftB;
	leftA.add(0.0);
    leftA.add(0.0);
    leftA.add(0.0);
    leftA.add(10.0);
    leftA.add(0.0);
    leftA.add(0.0);
    leftA.add(10.0);
    leftA.add(0.0);
    leftA.add(0.0);
    leftA.add(10.0);
    leftA.add(0.0);
    leftA.add(0.0);
    leftA.add(10.0);
    leftA.add(0.0);
    leftA.add(0.0);
    leftA.add(0.0);
    
    leftB.add(-95.0);
    leftB.add(0.0);
    leftB.add(10.0);
    leftB.add(0.0);
    leftB.add(0.0);
    leftB.add(10.0);
    leftB.add(0.0);
    leftB.add(0.0);
    leftB.add(10.0);
    leftB.add(0.0);
    leftB.add(0.0);
    leftB.add(10.0);
    leftB.add(0.0);
    leftB.add(0.0);
    leftB.add(0.0);
    leftB.add(0.0);
	int i = 0;
	while (true) {
	
		yarp::os::Bottle& msgT = torso.prepare();
        yarp::os::Bottle& msgL = left_arm.prepare();
        yarp::os::Bottle& msgR = right_arm.prepare();
        
		msgT.clear();   msgT.add("f");
        msgL.clear();   msgL.add("f");
        msgR.clear();   msgR.add("f");
        
        yarp::os::Bottle rnd;
        for (int j=0; j<3; j++)
            rnd.add( 100 * (2.0*rand()/RAND_MAX-1) );
        msgT.addList() = rnd;
        rnd.clear();
        for (int k=0; k<16; k++)
            rnd.add( 100 * (2.0*rand()/RAND_MAX-1) );
        msgL.addList() = rnd;
        rnd.clear();
        for (int l=0; l<16; l++)
            rnd.add( 100 * (2.0*rand()/RAND_MAX-1) );
        msgR.addList() = rnd;
		
		//printf("\nSending command to part controller: %s\n", msg.toString().c_str());
        std::cout << "torso: " << msgT.toString().c_str() << std::endl;
        std::cout << "left: " << msgL.toString().c_str() << std::endl;
        std::cout << "right: " << msgR.toString().c_str() << std::endl;
        
		torso.write();
        left_arm.write();
        right_arm.write();
		//msg.clear();
		
		i++;
		
		
		sleep(2);
	}
	
	return 1;	
}
