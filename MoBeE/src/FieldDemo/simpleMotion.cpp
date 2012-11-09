#include <unistd.h>
#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <yarp/sig/all.h>
#include <iostream.h>

int main(int argc, char *argv[])
{	
	yarp::os::Network net;
	yarp::os::BufferedPort<yarp::os::Bottle> //torso,
                                            //left_arm,
                                            right_arm;
	//torso.open("/torso_cmd");           net.connect("/torso_cmd","/MoBeE/torso/");
    right_arm.open("/right_arm_cmd");   net.connect("/right_arm_cmd","/MoBeE/right_arm/");
    //left_arm.open("/left_arm_cmd");     net.connect("/left_arm_cmd","/MoBeE/left_arm/");
	
	yarp::os::Bottle torsoA,torsoB;
	torsoA.add(10.0);   torsoA.add(0.0);    torsoA.add(0.0);
	torsoB.add(-10.0);  torsoB.add(0.0);    torsoB.add(0.0);
	
    yarp::os::Bottle leftA,leftB;
	leftA.add(0.0);
    leftA.add(0.0);
    leftA.add(0.0);
    leftA.add(0.0);
    leftA.add(0.0);
    leftA.add(0.0);
    leftA.add(0.0);
    leftA.add(0.0);
    leftA.add(0.0);
    leftA.add(0.0);
    leftA.add(0.0);
    leftA.add(0.0);
    leftA.add(0.0);
    leftA.add(0.0);
    leftA.add(0.0);
    leftA.add(0.0);
    
    leftB.add(1.0);
    leftB.add(1.0);
    leftB.add(1.0);
    leftB.add(1.0);
    leftB.add(1.0);
    leftB.add(1.0);
    leftB.add(1.0);
    leftB.add(1.0);
    leftB.add(1.0);
    leftB.add(1.0);
    leftB.add(1.0);
    leftB.add(1.0);
    leftB.add(1.0);
    leftB.add(1.0);
    leftB.add(1.0);
    leftB.add(1.0);
    
	int i = 0;
	while (true) {
	
        yarp::os::Bottle& msgR = right_arm.prepare();
   
        msgR.clear();   msgR.add("qatt");
        
        
        if (i%2 == 0)
            msgR.addList() = leftA;
        else
            msgR.addList() = leftB;
		
        //std::cout << "right: " << msgR.toString().c_str() << std::endl;
      
        right_arm.write();
		
		i++;
		
		
		sleep(15);
	}
	
	return 1;	
}
