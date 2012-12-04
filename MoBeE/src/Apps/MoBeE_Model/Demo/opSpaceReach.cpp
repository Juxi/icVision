/*******************************************************************
 ***               Copyright (C) 2012 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ***                                                             ***
 ***  This is a demonstration of using feedback operational      ***
 ***  space control within the MoBeE framework to reach toward   ***
 ***  random points using the iCub's right arm.                  ***
 ******************************************************************/
#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <yarp/sig/all.h>
#include <unistd.h>
#include <time.h>

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
    
    // connect to the world model RPC port
    yarp::os::RpcClient world_rpc;
    world_rpc.open("/world_port_rpc");
    net.connect("/world_port_rpc","/MoBeE/world");
    
    // connect to the controller RPC port
    yarp::os::RpcClient right_arm_rpc;
    right_arm_rpc.open("/right_arm_rpc");
    net.connect("/right_arm_rpc","/MoBeE/right_arm/rpc");
    
    // prepare some bottles to poll the MoBeE model
    yarp::os::Bottle get,state;
    get.addVocab(yarp::os::Vocab::encode("get"));
    get.addString("right_hand");
    
    // position vector and normal of the hand marker
    double pHand[3],nHand[3],err[6];
    
    // connect to the controller command port
    yarp::os::BufferedPort<yarp::os::Bottle> right_arm_cmd;
    right_arm_cmd.open("/right_arm_cmd");
    net.connect("/right_arm_cmd","/MoBeE/right_arm/cmd");
    
    // gains for the control signal
    double  forceMagnitude = 10000.0,
            torqueMagnitude = 10000.0;
    
    // reach to each target point
    for (int i=0; i<100; i++)
    {
        // pick a random target to reach to
        double p[3];
        p[0] = x_min + (double)rand()/RAND_MAX * (x_max-x_min);
        p[1] = y_min + (double)rand()/RAND_MAX * (y_max-y_min);
        p[2] = z_min + (double)rand()/RAND_MAX * (z_max-z_min);
        
        // visualize it in the world model
        yarp::os::Bottle cmd, rsp;
        cmd.addVocab(yarp::os::Vocab::encode("mk"));
        cmd.addVocab(yarp::os::Vocab::encode("sph"));
        cmd.addDouble(0.01);
        cmd.addDouble(p[0]);
        cmd.addDouble(p[1]);
        cmd.addDouble(p[2]);
        world_rpc.write(cmd,rsp);
        printf("\ncommand: %s\n", cmd.toString().c_str());
        printf("  response: %s\n\n", rsp.toString().c_str());
        
        // get the name of the newly inserted object
        yarp::os::ConstString objectName = rsp.get(0).asString();
        
        // clean up the bottles for reuse
        cmd.clear();
        rsp.clear();
            
        // reach to the point p for an arbitrary amount of time
        int count = 0;
        while (count < 1000)
        {
            // get the state of the hand
            right_arm_rpc.write(get,state);
            pHand[0] = state.get(0).asDouble();
            pHand[1] = state.get(1).asDouble();
            pHand[2] = state.get(2).asDouble();
            nHand[0] = state.get(3).asDouble();
            nHand[1] = state.get(4).asDouble();
            nHand[2] = state.get(5).asDouble();
            
            // error vector from hand to target
            err[0] = p[0] - pHand[0];
            err[1] = p[1] - pHand[1];
            err[2] = p[2] - pHand[2];
            
            // and normalize it
            double mErr = sqrt(pow(err[0],2) + pow(err[1],2) + pow(err[2],2));
            err[0] /= mErr;
            err[1] /= mErr;
            err[2] /= mErr;
            
            // angular error between hand normal and error vector (radians)
            //double theta = acos( err[0]*nHand[0] + err[1]*nHand[1] + err[2]*nHand[2] );
            
            // angular error direction ... (nHand)x(err)
            double t[3];
            t[0] = nHand[1]*err[2] - nHand[2]*err[1];
            t[1] = nHand[2]*err[0] - nHand[0]*err[2];
            t[2] = nHand[0]*err[1] - nHand[1]*err[0];
            
            // and normalize it
            double mt = sqrt(pow(t[0],2) + pow(t[1],2) + pow(t[2],2));
            t[0] /= mt;
            t[1] /= mt;
            t[2] /= mt;
            
            // build up the command bottle
            yarp::os::Bottle& opSpaceForceAndTorque = right_arm_cmd.prepare();
            opSpaceForceAndTorque.clear();
            opSpaceForceAndTorque.addVocab(yarp::os::Vocab::encode("opsp"));
            opSpaceForceAndTorque.addString("right_hand");
            
            yarp::os::Bottle cmd;
            cmd.addDouble(forceMagnitude*err[0]);
            cmd.addDouble(forceMagnitude*err[1]);
            cmd.addDouble(forceMagnitude*err[2]);
            
            // either with no torque
            cmd.addDouble(0.0);
            cmd.addDouble(0.0);
            cmd.addDouble(0.0);
            
            // or try some torque
            //cmd.addDouble(torqueMagnitude*t[0]);
            //cmd.addDouble(torqueMagnitude*t[1]);
            //cmd.addDouble(torqueMagnitude*t[2]);
            
            opSpaceForceAndTorque.addList() = cmd;
            
            printf("Sending control command: %s\n", opSpaceForceAndTorque.toString().c_str());
            right_arm_cmd.write();
            
            usleep(20000);
            count++;
        }
        
        // remove the sphere from the model
        cmd.addVocab(yarp::os::Vocab::encode("rm"));
        cmd.addString(objectName);
        world_rpc.write(cmd,rsp);
        printf("\ncommand: %s\n", cmd.toString().c_str());
        printf("  response: %s\n\n", rsp.toString().c_str());
        
    }
	
	return 1;
}
