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
	
    yarp::os::Bottle A,B,C,D;
	A.add(0.5);
    A.add(0.1);
    A.add(0.2);
    A.add(0.0);
    A.add(0.0);
    A.add(0.0);
    A.add(0.0);
    A.add(0.0);
    A.add(0.0);
    A.add(0.0);
    A.add(0.0);
    A.add(0.0);
    A.add(0.0);
    A.add(0.0);
    A.add(0.0);
    A.add(0.0);
    
    B.add(0.5);
    B.add(0.1);
    B.add(0.2);
    B.add(0.0);
    B.add(1.0);
    B.add(0.0);
    B.add(0.0);
    B.add(0.0);
    B.add(0.0);
    B.add(0.0);
    B.add(0.0);
    B.add(0.0);
    B.add(0.0);
    B.add(0.0);
    B.add(0.0);
    B.add(0.0);
    
    C.add(0.5);
    C.add(0.1);
    C.add(0.2);
    C.add(1.0);
    C.add(0.0);
    C.add(0.0);
    C.add(0.0);
    C.add(0.0);
    C.add(0.0);
    C.add(0.0);
    C.add(0.0);
    C.add(0.0);
    C.add(0.0);
    C.add(0.0);
    C.add(0.0);
    C.add(0.0);
    
    D.add(0.5);
    D.add(0.1);
    D.add(0.2);
    D.add(1.0);
    D.add(1.0);
    D.add(0.0);
    D.add(0.0);
    D.add(0.0);
    D.add(0.0);
    D.add(0.0);
    D.add(0.0);
    D.add(0.0);
    D.add(0.0);
    D.add(0.0);
    D.add(0.0);
    D.add(0.0);
    
	int i = 0;
	while (true) {
	
        /*yarp::os::Bottle& msgA = right_arm.prepare();
        msgA.clear();
        msgA.add("qatt");
        msgA.addList() = A;
        right_arm.write();
        sleep(15);
        
        yarp::os::Bottle& msgB = right_arm.prepare();
        msgB.clear();
        msgB.add("qatt");
        msgB.addList() = B;
        right_arm.write();
        sleep(15);*/
        
        yarp::os::Bottle& msgC = right_arm.prepare();
        msgC.clear();
        msgC.add("qatt");
        msgC.addList() = D;
        right_arm.write();
        sleep(15);
        
        yarp::os::Bottle& msgD = right_arm.prepare();
        msgD.clear();
        msgD.add("qatt");
        msgD.addList() = C;
        right_arm.write();
        sleep(15);
	}
	
	return 1;	
}
