#include <QCoreApplication>
#include "armController.h"
#include "headController.h"
#include "torsoController.h"

#define VOCAB_QUIT VOCAB4('q','u','i','t')		// stop controller
#define VOCAB_INIT VOCAB4('i','n','i','t')		// goto initial pose
#define VOCAB_GO   VOCAB2('g','o')				// go
#define VOCAB_BACK VOCAB2('b','k')				// go back ...

//#define VOCAB_PREGRASP VOCAB3('p','r','e')	// goto pregrasp pose (position control)
//#define VOCAB_GRASP VOCAB3('c','l','s')		// close the hand	(velocity control)
//#define VOCAB_UNGRASP VOCAB3('o','p','n')	// open the hand	(velocity control)

void initialTrajectories(HeadController *ctrl_head, TorsoController *ctrl_torso, ArmController *left, ArmController *right);
void initializeWrestlingTrajectories(HeadController *ctrl_head, TorsoController *ctrl_torso, ArmController *left, ArmController *right);

int main(int argc, char *argv[])
{
	/******************************************
	 *** Get arguments off the command line ***
	 ******************************************/
	yarp::os::Property config;
	config.fromCommand(argc, argv);
	
	if ( !config.check("robot") )	// the name of the robot to connect to
	{
		printf("Please specify a robot name using '--robot yourRobotName --data file.csv'.\n");
		return 1;
	}
	
	std::string name = "/armWrestlingController";	// the name of the RPC server
	if ( config.check("name") )  { name = config.find("name").asString().c_str(); } 
	
	qreal errTolerance = 0.8;	// 0-1, where smaller numbers favor precision
	if ( config.check("tolerance") )  { errTolerance = config.find("tolerance").asDouble(); }
	
	qreal attFactor = 4.0;	// 0-inf larger numbers cause the controller to approach target points more slowly
	if ( config.check("attenuation") )  { attFactor = config.find("attenuation").asDouble(); }
	
	qreal accel = 10.0;	// reference acceleration for the yarp remote control board interface
	if ( config.check("acceleration") )  { accel = config.find("acceleration").asDouble(); }
	
	qreal vel = 10.0;	// reference velocity for the yarp remote control board interface
	if ( config.check("velocity") )  { vel = config.find("velocity").asDouble(); }
	
	std::string csvfile = "";	// the name of the RPC server
	if ( config.check("data") )  { csvfile = config.find("data").asString().c_str(); } 

	try
	{
		// Initializing the YARP connections	
		// just needed once to move to save pose
		ArmController ctrl_leftArm ( errTolerance, attFactor );
		ctrl_leftArm.connect( config.find("robot").asString().c_str(), "left_arm", accel, vel);

		// just needed once to move to pose
		TorsoController ctrl_torso ( errTolerance, attFactor );
		ctrl_torso.connect( config.find("robot").asString().c_str(), "torso", accel, vel);
		
		// controlling the right arm
		HeadController ctrl_head ( 0.4, 3.0 );
		ctrl_head.connect( config.find("robot").asString().c_str(), "head", accel, vel);

		// controlling the right arm
		ArmController ctrl_rightArm ( 0.4, 2.0 );
		ctrl_rightArm.connect( config.find("robot").asString().c_str(), "right_arm", accel, vel);
		
		
		// TODO Initialize the trajectories (read from csv?)
//		initializeTrajectories(csvfile, &ctrl_head, &ctrl_torso, &ctrl_leftArm, &ctrl_rightArm);
		
		yarp::os::Network yarp;
		yarp::os::RpcServer port;
		printf("Opening RPC port..");
		port.open(name.c_str());
		printf(". DONE!\n");
		
		
		// main loop, handling messages from RPC
		bool finished = false;
		while (! finished) {

			printf("Waiting for a message...\n");
			yarp::os::Bottle cmd;
			yarp::os::Bottle response;
			port.read(cmd,true);
			
			int command = cmd.get(0).asVocab();
			printf("Got message: %s\n", cmd.toString().c_str());

			switch( command ) {
				/////////////////					
				case VOCAB_INIT:
					system("echo \"set all shy\" | yarp rpc /icub/face/emotions/in");
					
					initialTrajectories(&ctrl_head, &ctrl_torso, &ctrl_leftArm, &ctrl_rightArm);
					
					// maybe replace by start?!, probably should .. hmm... or at least do one by one ...
					ctrl_rightArm.initialPose();
					ctrl_leftArm.initialPose();
					ctrl_torso.initialPose();					
					ctrl_head.initialPose();

					system("echo \"set all ang\" | yarp rpc /icub/face/emotions/in");	

					response.addString("OK");					
					break;
				
				/////////////////
				case VOCAB_GO:
					system("echo \"set all evi\" | yarp rpc /icub/face/emotions/in");
					
					initializeWrestlingTrajectories(&ctrl_head, &ctrl_torso, &ctrl_leftArm, &ctrl_rightArm);					

					//ctrl_torso.start();	
					ctrl_head.start();
					ctrl_rightArm.start();	
					
					while (ctrl_rightArm.isRunning() || ctrl_head.isRunning()) usleep(150);

					response.addString("OK");

					usleep(500*1000);
					system("echo \"set all hap\" | yarp rpc /icub/face/emotions/in");					

					break;

				/////////////////
				case VOCAB_BACK:
					system("echo \"set all evi\" | yarp rpc /icub/face/emotions/in");
					
					initializeWrestlingTrajectories(&ctrl_head, &ctrl_torso, &ctrl_leftArm, &ctrl_rightArm);					
					ctrl_head.reverseTrajectory();
					ctrl_rightArm.reverseTrajectory();
					
					//ctrl_torso.start();	
					ctrl_head.start();
					ctrl_rightArm.start();	
					
					while (ctrl_rightArm.isRunning() || ctrl_head.isRunning()) usleep(150);
					
					response.addString("OK");
					
					break;
					
					
				/////////////////			
				case VOCAB_QUIT:
					response.addString("OK");					
					ctrl_rightArm.stop();
					ctrl_leftArm.stop();
					ctrl_head.stop();			
					ctrl_torso.stop();
					finished = true;
					break;

				/////////////////					
				default:
					response.addString("Unknown Command.  Use 'init' to initialize, 'quit' to close."); //The ints should be a speed 0-100");
					break;
			}
			
			printf("Sending reply: %s\n", response.toString().c_str());
			port.reply(response);
		}
	}
	catch ( QString e ) {
		printf("%s", e.toStdString().c_str());
	}
	catch (...) {
		printf("All shits fucked up, mate!");
	}
	return 0;
}


void initialTrajectories(HeadController *ctrl_head, TorsoController *ctrl_torso, ArmController *left, ArmController *right) {
	
	ctrl_torso->clearTrajectory();
	ctrl_head->clearTrajectory();
	left->clearTrajectory();
	right->clearTrajectory();
	
	
	// from -7.000077,1.999959,-47.0001, 1.000166,-0.000106,-0.004308,
	float h[NUMBEROFHEADJOINTS] = { -7.0, 2.0, -47, 1, 0.0, 0.0 };
	ctrl_head->addControlPoint(h);
	
	//-36.967033,0.043956,9.978022
	float t[NUMBEROFTORSOJOINTS] = {-37, 0.0, 10};
	ctrl_torso->addControlPoint(t);
	
	//-1.010989,63.005495,16.914835,25.010989,-0.000425,0.000204,-0.000082,14.857157,29.992439,0.0,-0.155642,6.570995,5.670498,7.044929,2.605364,8.758943
	float l[NUMBEROFARMJOINTS] = { -1.0, 63, 17, 25, 0.0, 0.0, 0.0, 15, 30, 0, 0, 6.5, 5.6, 7, 2.6, 9.0 };
	left->addControlPoint(l);
	
	// 	-52.967033,53.010989,-21.036703,91.032967,-0.000425,0.000204,-0.000082,14.99995,30.004151,4.34551,4.902724,3.563704,-0.152672,5.564202,7.027027,5.3
	float r [NUMBEROFARMJOINTS] = {-52.0, 53, -21, 91, 0, 0, 0, 
					25,	   30, 4.3, 37, 24.5, 31.0, 25, 38, 11 };		
		// fingers jnt 7	8	9	10	   11	12	13	14	15
		// for juergen's closed hand idea
	
	// original 15, 30, 4.3, 4.9, 3.5, -0.2, 5.5, 7, 5.3};
	right->addControlPoint(r);
}


void initializeWrestlingTrajectories(HeadController *ctrl_head, TorsoController *ctrl_torso, ArmController *left, ArmController *right) {
	
	ctrl_torso->clearTrajectory();
	ctrl_head->clearTrajectory();
	left->clearTrajectory();
	right->clearTrajectory();
	
	
	// from -7.000077,1.999959,-47.0001, 1.000166,-0.000106,-0.004308,
	float h[NUMBEROFHEADJOINTS] = { -7.0, 2.0, -47, 1, 0.0, 0.0 };
	ctrl_head->addControlPoint(h);
	
	//-36.967033,0.043956,9.978022
	float t[NUMBEROFTORSOJOINTS] = {-37, 0.0, 10};
	ctrl_torso->addControlPoint(t);
	
	//-1.010989,63.005495,16.914835,25.010989,-0.000425,0.000204,-0.000082,14.857157,29.992439,0.0,-0.155642,6.570995,5.670498,7.044929,2.605364,8.758943
	float l[NUMBEROFARMJOINTS] = { -1.0, 63, 17, 25, 0.0, 0.0, 0.0, 15, 30, 0, 0, 6.5, 5.6, 7, 2.6, 9.0 };
	left->addControlPoint(l);
	
	// 	-52.967033,53.010989,-21.036703,91.032967,-0.000425,0.000204,-0.000082,14.99995,30.004151,4.34551,4.902724,3.563704,-0.152672,5.564202,7.027027,5.3
	float r [NUMBEROFARMJOINTS] = {-52.0, 53, -21, 91, 0, 0, 0, 
		25,	   30, 4.3, 37, 24.5, 31.0, 25, 38, 11 };		
	// fingers jnt 7	8	9	10	   11	12	13	14	15
	// for juergen's closed hand idea
	
	// original 15, 30, 4.3, 4.9, 3.5, -0.2, 5.5, 7, 5.3};
	right->addControlPoint(r);
	
	// end position...
	
	// -7.000077,1.999959,-21.000071,-7.999861,0.001599,-0.000899,
	float h2[NUMBEROFHEADJOINTS] = { -7.0, 2.0, -21, -8, 0.0, 0.0 };
	ctrl_head->addControlPoint(h2);
	
	//  -52.967033,53.098901,35.93033,89.978022,-3.00041,0.000204,-0.000082,14.99995,30.004151,3.325102,4.902724,3.975226,-0.152672,7.120623,7.027027,5.792624
	float r2[NUMBEROFARMJOINTS] = { -52.0, 53, 38, 91, 0, 0, 0,
		25,	   30, 4.3, 37, 24.5, 31.0, 25, 38, 11 };
	// fingers jnt 7	8	9	10	   11	12	13	14	15
	right->addControlPoint(r2);
}


void initializeTrajectories(std::string csvfile, HeadController *ctrl_head, TorsoController *ctrl_torso, ArmController *left, ArmController *right) {
	// from -7.000077,1.999959,-47.0001, 1.000166,-0.000106,-0.004308,
	float h[NUMBEROFHEADJOINTS] = { -7.0, 2.0, -47, 1, 0.0, 0.0 };
	ctrl_head->addControlPoint(h);
	
	//-36.967033,0.043956,9.978022
	float t[NUMBEROFTORSOJOINTS] = {-37, 0.0, 10};
	ctrl_torso->addControlPoint(t);
	
	//-1.010989,63.005495,16.914835,25.010989,-0.000425,0.000204,-0.000082,14.857157,29.992439,0.0,-0.155642,6.570995,5.670498,7.044929,2.605364,8.758943
	float l[NUMBEROFARMJOINTS] = { -1.0, 63, 17, 25, 0.0, 0.0, 0.0, 15, 30, 0, 0, 6.5, 5.6, 7, 2.6, 9.0 };
	left->addControlPoint(l);
	
	// 	-52.967033,53.010989,-21.036703,91.032967,-0.000425,0.000204,-0.000082,14.99995,30.004151,4.34551,4.902724,3.563704,-0.152672,5.564202,7.027027,5.3
	float r [NUMBEROFARMJOINTS] = {-52.0, 53, -21, 91, 0, 0, 0, 15, 30, 4.3, 4.9, 3.5, -0.2, 5.5, 7, 5.3};
	right->addControlPoint(r);
	
	// end position...
	
	// -7.000077,1.999959,-21.000071,-7.999861,0.001599,-0.000899,
	float h2[NUMBEROFHEADJOINTS] = { -7.0, 2.0, -21, -8, 0.0, 0.0 };
	ctrl_head->addControlPoint(h2);
	
	//  -52.967033,53.098901,35.93033,89.978022,-3.00041,0.000204,-0.000082,14.99995,30.004151,3.325102,4.902724,3.975226,-0.152672,7.120623,7.027027,5.792624
	float r2[NUMBEROFARMJOINTS] = {-52.0, 53,  38, 91, 0, 0, 0, 15, 30, 4.3, 4.9, 3.5, -0.2, 5.5, 7, 5.3};
	right->addControlPoint(r2);
	
	
}

