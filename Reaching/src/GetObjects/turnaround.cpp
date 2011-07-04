// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

/*

This is an example of using a specialized RpcClient port to send
messages and receive replies.  Regular YARP ports can do this as well,
but use of RpcServer/RpcClient allows for better
run-time checking of port usage to catch mistakes.

 */

#include <stdio.h>
#include <math.h>
#include <strings.h>

//#include <yarp/sig/Vector.h>
//#include <yarp/sig/Matrix.h>
#include <yarp/os/all.h>
#include <yarp/math/Math.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::math;

// simulator world frame to robot reference frame trandformation
// [ 0 -1 0 0; 0 0 1 0.5484; -1 0 0 -0.04; 0 0 0 1 ]
Matrix world2robot;
Matrix robot2world;


int main(int argc, char *argv[]) {
    if (argc<2) {
        fprintf(stderr, "Please supply a port name for the server, e.g. /icubSim/world \n");
        return 1;
    }

    Network yarp;
    const char *client_name = "/Juggler/ssph";
    const char *server_name = argv[1];

    RpcClient port;
    port.open(client_name);

    robot2world = Matrix(4,4);
    double row0[4] = { 0.0, -1.0, 0.0, 0.0};
    double row1[4] = { 0.0,  0.0, 1.0, 0.5484};
    double row2[4] = {-1.0,  0.0, 0.0,-0.04};
    double row3[4] = { 0.0,  0.0, 0.0, 1.0};
    robot2world.setRow(0,Vector(4,row0));
    robot2world.setRow(1,Vector(4,row1));
    robot2world.setRow(2,Vector(4,row2));
    robot2world.setRow(3,Vector(4,row3));
    world2robot = luinv(robot2world);

    double a = 0; 
    // double x=cos(a);double y=0.5;double z=sin(a);
    double x = -0.3;
    double y = -0.1+0.1*cos(a);
    double z = +0.1+0.1*sin(a);
    double pos[4];
    pos [0] = x; pos [1] = y; pos [2] = z; pos [3] = 1;
    Vector posRobot(4,pos);
    Vector posWorld = robot2world*posRobot;    

    while (true) {
	if (port.getOutputCount()==0) {
	  printf("Trying to connect to %s\n", server_name);
	  yarp.connect(client_name,server_name);
      Time::delay(1);
      Bottle cmd;
      
	  cmd.addString("world");
	  cmd.addString("mk");
	  cmd.addString("ssph");
	  cmd.addDouble(0.02);
	  cmd.addDouble(posWorld[0]);
	  cmd.addDouble(posWorld[1]);
	  cmd.addDouble(posWorld[2]);
	  cmd.addDouble(0);
	  cmd.addDouble(1);
	  cmd.addDouble(0);
      printf("Sending message... %s\n", cmd.toString().c_str());
      Bottle response;
	  port.write(cmd,response);
	  printf("Got response: %s\n", response.toString().c_str());     
	} else {
#if 0
    a = a+M_PI/36.0;if (a>2*M_PI) a=a-2*M_PI;
    x = cos(a); y = 0.5; z = sin(a);
    Bottle cmd;
    cmd.addString("world");
    cmd.addString("set");
    cmd.addString("ssph");
    cmd.addInt(1);
    cmd.addDouble(x);
    cmd.addDouble(y);
    cmd.addDouble(z);
    printf("Sending message... %s\n", cmd.toString().c_str());
    Bottle response;
    port.write(cmd,response);
    printf("Got response: %s\n", response.toString().c_str());
    printf("Number of elements: %i\n",response.size());
    for (int i=0;i<response.size();++i)
    {
        printf("  Item %i: %s",i,response.get(i).toString().c_str());
        if (response.get(i).isInt()) printf("[Int=%i]",response.get(i).asInt());
        if (response.get(i).isString()) printf("[String=%s]",response.get(i).asString().c_str());
        if (response.get(i).isDouble()) printf("[Double=%f]",response.get(i).asDouble());
        if (response.get(i).isList()) printf("[List]");//Bottle * 	asList () // recursive
        if (response.get(i).isVocab()) printf("[Vocab=%i]",response.get(i).asVocab());
        if (response.get(i).isBlob()) printf("[Blob]");//const char * 	asBlob ()
        printf("\n");
    };
#else
    //world get ssph 1
    //Response: 0.5 0.7 0.0
    //world set ssph 1 0.6 0.8 0.0
    //Response: [ok]
    Bottle cmd;
    cmd.addString("world");
    cmd.addString("get");
    cmd.addString("ssph");
    cmd.addInt(1);
    printf("Sending message... %s\n", cmd.toString().c_str());
    Bottle response;
    port.write(cmd,response);
    printf("Got response: %s\n", response.toString().c_str());
    printf("Number of elements: %i\n",response.size());
    for (int i=0;i<response.size();++i)
    {
        printf("  Item %i: %s",i,response.get(i).toString().c_str());
        if (response.get(i).isInt()) printf("[Int=%i]",response.get(i).asInt());
        if (response.get(i).isString()) printf("[String=%s]",response.get(i).asString().c_str());
        if (response.get(i).isDouble()) printf("[Double=%f]",response.get(i).asDouble());
        if (response.get(i).isList()) printf("[List]");//Bottle * 	asList () // recursive
        if (response.get(i).isVocab()) printf("[Vocab=%i]",response.get(i).asVocab());
        if (response.get(i).isBlob()) printf("[Blob]");//const char * 	asBlob ()
        printf("\n");
    };
    if (response.size()==3)
    {
        //double pos[4];
        pos [0] = response.get(0).asDouble(); pos [1] = response.get(1).asDouble(); pos [2] = response.get(2).asDouble(); pos [3] = 1;
        posRobot = world2robot*Vector(4,pos);
        x = posRobot[0];
        y = posRobot[1];
        z = posRobot[2];
        // turn around the robot
        // a = atan2(z,x);
        // printf("before x=%f,y=%f,z=%f,a=%f\n",x,y,z,a);
        // a = a+M_PI/36.0;if (a>2.0*M_PI) a=a-2.0*M_PI;
        // x = cos(a); y = y; z = sin(a);
        // like the tutorial cartesian interface movement for the hand.
        // xd[0]=-0.3;
        // xd[1]=-0.1+0.1*cos(2.0*M_PI*0.1*(t-t0));
        // xd[2]=+0.1+0.1*sin(2.0*M_PI*0.1*(t-t0));            
        a = atan2(z-0.1,y+0.1);
        printf("before x=%f,y=%f,z=%f,a=%f\n",x,y,z,a);
        a = a+2.0*M_PI/360.0;if (a>2.0*M_PI) a=a-2.0*M_PI;  
        x = x;
        y = -0.1+0.1*cos(a);
        z = +0.1+0.1*sin(a);    
        printf("after x=%f,y=%f,z=%f,a=%f\n",x,y,z,a);
        pos [0] = x; pos [1] = y; pos [2] = z; pos [3] = 1;
        posWorld = robot2world*Vector(4,pos);    

        cmd.clear();
        cmd.addString("world");
        cmd.addString("set");
        cmd.addString("ssph");
        cmd.addInt(1);
        cmd.addDouble(posWorld[0]);
        cmd.addDouble(posWorld[1]);
        cmd.addDouble(posWorld[2]);
        printf("Sending message... %s\n", cmd.toString().c_str());
        response.clear();
        port.write(cmd,response);
        printf("Got response: %s\n", response.toString().c_str());
        printf("Number of elements: %i\n",response.size());
        for (int i=0;i<response.size();++i)
        {
            printf("  Item %i: %s",i,response.get(i).toString().c_str());
            if (response.get(i).isInt()) printf("[Int=%i]",response.get(i).asInt());
            if (response.get(i).isString()) printf("[String=%s]",response.get(i).asString().c_str());
            if (response.get(i).isDouble()) printf("[Double=%f]",response.get(i).asDouble());
            if (response.get(i).isList()) printf("[List]");//Bottle * 	asList () // recursive
            if (response.get(i).isVocab()) printf("[Vocab=%i]",response.get(i).asVocab());
            if (response.get(i).isBlob()) printf("[Blob]");//const char * 	asBlob ()
            printf("\n");
        };
   };
#endif
	}
	Time::delay(0.25);
    }
}
