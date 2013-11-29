#include "moverPosition.h"
#include "moverMinJerkLinear.h"
#include "moverMinJerkForward.h"
#include "moverVelocityForward.h"
#include "mover.h"

#include <string>
#include <iostream>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;

#define VOCAB_GO VOCAB2('g','o')			// run trajectory
#define VOCAB_MASK VOCAB4('m','a','s','k')	// get mask
#define VOCAB_POSE VOCAB4('p','o','s','e')	// get pose
#define VOCAB_GET  VOCAB3('g','e','t')		// get 
#define VOCAB_SPD  VOCAB3('s','p','d')		// set reference speed
#define VOCAB_ACC  VOCAB3('a','c','c')		// set reference accelleration
#define VOCAB_FWD  VOCAB3('f','w','d')		// set number of forward steps
#define VOCAB_HELP VOCAB4('h','e','l','p')	// display help
#define VOCAB_BLINK VOCAB4('b','l','n','k')	// blink eyes
#define VOCAB_DIST_THRESHOLD VOCAB3('t','h','r')   // distance threshold
#define VOCAB_OK VOCAB2('o','k')
#define VOCAB_KP VOCAB2('k', 'p')
#define VOCAB_FAIL VOCAB4('f','a','i','l')
#define VOCAB_STOP VOCAB4('s','t','o','p') // cancel ongoing movement
#define VOCAB_MOVE_MODE VOCAB4('m','o','d','e')
#define VOCAB_MIN_TRAJ_TIME VOCAB4('m','i','n','t')
#define VOCAB_MIN_ABS_VEL VOCAB3('m','a','v')

#define MAX_REFERENCE_SPEED 40.
#define MAX_REFERENCE_ACCELERATION 1000.
#define HARD_MIN_TRAJ_TIME 0.1
#define HARD_MAX_MIN_ABS_VEL 2.0 // controller should not do bang-bang control above this velocity

typedef MoverVelocityForward mover_type;
//typedef MoverMinJerkForward mover_type;
//typedef MoverMinJerkLinear mover_type;
//typedef MoverPosition mover_type;
//typedef Mover mover_type;


int main(int argc, char *argv[]) {
	Network yarp;
	Property command;
	command.fromCommand(argc, argv);
	Property file;
	Bottle settings;
	bool hasConfigFile = false;

	string fname;
	if (command.check("file")) {
		fname = command.find("file").asString();
		hasConfigFile = file.fromConfigFile(fname.c_str());
		settings = file.findGroup("SETTINGS");
	}

	// the following values are read from the configuration file, but can be overwritten from the command line
	string robot = "icubSim"; // default is simulator
	if ( settings.check("robot") )  { robot = settings.find("robot").asString().c_str(); }
	if ( command.check("robot") )  { robot = command.find("robot").asString().c_str(); }
	
	string portname = "/" + robot + "/" + "mover"; // the name of the RPC server
	if ( settings.check("port") )  { portname = settings.find("port").asString().c_str(); } 
	if ( command.check("port") )  { portname = command.find("port").asString().c_str(); } 

	double refSpd = 10.0;	// reference velocity for position control moves
	if ( settings.check("refspeed") )  { refSpd = settings.find("refspeed").asDouble(); }
	if ( command.check("refspeed") )  { refSpd = command.find("refspeed").asDouble(); }
	refSpd = max(0., min(refSpd, MAX_REFERENCE_SPEED));

	double refAcc = 1000.0;	// reference acceleration for the yarp remote control board interface
	if ( settings.check("acceleration") )  { refAcc = settings.find("acceleration").asDouble(); }
	if ( command.check("acceleration") )  { refAcc = command.find("acceleration").asDouble(); }
	refAcc = max(0., min(refAcc, MAX_REFERENCE_ACCELERATION));

	int fwdSteps = 40;	// forward steps for moverMinJerkForward
	if ( settings.check("fwdsteps") )  { fwdSteps = settings.find("fwdsteps").asDouble(); }
	if ( command.check("fwdsteps") )  { fwdSteps = command.find("fwdsteps").asDouble(); }
	fwdSteps = max(1, fwdSteps);

	double distThreshold = 1;	// default distance threshold before moving to the next pose in degrees
	if ( settings.check("distthreshold") )  { distThreshold = settings.find("distthreshold").asDouble(); }
	if ( command.check("distthreshold") )  { distThreshold = command.find("distthreshold").asDouble(); }

	double finalDistThreshold = 0.1;	// distance threshold for final pose
	if ( settings.check("finaldistthreshold") )  { finalDistThreshold = settings.find("finaldistthreshold").asDouble(); }
	if ( command.check("finaldistthreshold") )  { finalDistThreshold = command.find("finaldistthreshold").asDouble(); }

	double stepTimeout = 2;	// default step timeout in seconds
	if ( settings.check("steptimeout") )  { stepTimeout = settings.find("steptimeout").asDouble(); }
	if ( command.check("steptimeout") )  { stepTimeout = command.find("steptimeout").asDouble(); }

	double trajTimeout = 10; // default trajectory timeout in seconds
	if ( settings.check("trajtimeout") )  { trajTimeout = settings.find("trajtimeout").asDouble(); }
	if ( command.check("trajtimeout") )  { trajTimeout = command.find("trajtimeout").asDouble(); }

	double minTrajTime = 0.5; // default minimum trajectory time
	if ( settings.check("mintrajtime") )  { minTrajTime = settings.find("mintrajtime").asDouble(); }
	if ( command.check("mintrajtime") )  { minTrajTime = command.find("mintrajtime").asDouble(); }

	double kp = 2; // default proporitional part of P-controller
	if ( settings.check("kp") )  { kp = settings.find("kp").asDouble(); }
	if ( command.check("kp") )  { kp = command.find("kp").asDouble(); }

	double minabsvel = 0.0; // default minimum velocity before switching to bang-bang controler
	if ( settings.check("minabsvel") )  { minabsvel = settings.find("minabsvel").asDouble(); }
	if ( command.check("minabsvel") )  { minabsvel = command.find("minabsvel").asDouble(); }


	int moveMode = VOCAB_MODE_VELOCITY; // default mode is velocity
	if ( settings.check("movemode") )  { moveMode = settings.find("movemode").asDouble(); }
	if ( command.check("movemode") )  { moveMode = command.find("movemode").asDouble(); }

		string vSkinRpcPort = "/virtualSkin/filterRpc";
	if ( settings.check("vskinrpc") )  { vSkinRpcPort = settings.find("vskinrpc").asString().c_str(); }
	if ( command.check("vskinrpc") )  { vSkinRpcPort = command.find("vskinrpc").asString().c_str(); }

	string vSkinStatus = "/virtualSkin/filterStatus";
	if ( settings.check("vskinstatus") )  { vSkinStatus = settings.find("vskinstatus").asString().c_str(); }
	if ( command.check("vskinstatus") )  { vSkinStatus = command.find("vskinstatus").asString().c_str(); }

	Bottle defaultMasks;
	if ( settings.check("defaultmasks") )  { defaultMasks = settings.findGroup("defaultmasks"); }
	if ( command.check("defaultmasks") )  { defaultMasks = command.findGroup("defaultmasks"); }

	string rawFacePort = "/" + robot + "/face/raw/in";
	if ( settings.check("rawfaceport") )  { rawFacePort = settings.find("rawfaceport").asString(); }
	if ( command.check("rawfaceport") )  { rawFacePort = command.find("rawfaceport").asString().c_str(); }
		
	// parse partnames
	vector<string> partnames;
	bool hasParts = false;
	Bottle yParts;
	if ( settings.check("parts") )  { yParts = settings.findGroup("parts"); hasParts = true; }
	if ( command.check("parts") )  { yParts = command.findGroup("parts"); hasParts = true; }
	if (!hasParts) {
		partnames.push_back("head"); partnames.push_back("torso"); // default parts
	} else {
		for (int ipart=1; ipart<yParts.size(); ipart++) { partnames.push_back(yParts.get(ipart).toString().c_str()); }
	}

	vector<vector<vector<double> > > thisTraj;
	double thisDistThreshold, thisStepTimeout, thisTrajTimeout;
	bool success;

	// open mover and initialize
	mover_type mover;
	if (!mover.init(robot, partnames)) { return 0; }
	mover.setRefSpeed(refSpd);
	mover.setRefAcceleration(refAcc);
	mover.setFwdSteps(fwdSteps);
	mover.setMinTrajTime(minTrajTime);
	mover.setMode(moveMode);
	mover.setKp(kp);
	mover.setMinAbsVel(minabsvel);
	
	// parse and set default masks
	if (defaultMasks.size() > 1) {
		for (int iMask=1; iMask<defaultMasks.size(); iMask++) {
			if (defaultMasks.get(iMask).asString() == "eyes") {	mover.maskEyes(); cout << "Eyes are masked" << endl; }
			else if (defaultMasks.get(iMask).asString() == "fingers") { mover.maskFingers(); cout << "Fingers are masked" << endl; }
			else if (defaultMasks.get(iMask).asString() == "head") { mover.maskHead(); cout << "Head is masked" << endl; }
			else {cout << "Warning: invalid mask \"" << defaultMasks.get(iMask).asString().c_str() << "\" cannot be set." << endl << endl; }
		}
	}

	// connect face for monitoring
	if (!mover.connectFace(rawFacePort))
	{ cout << "Warning: Facial Expressions disabled! " << endl << endl; }

	// connect virtual skin
	if (!mover.connnectVSkin(vSkinRpcPort, vSkinStatus))
	{ cout << "Warning: Virtual Skin interaction not established! " << endl << endl; }

	// open rpc port
	RpcServer port;
	port.open(portname.c_str());

	// open monitor port
	if (!mover.connectMonitor())
	{ cout << "Warning: Could not open monitor port " << endl << endl; }

	while (true) {
		cout << "Waiting for a message..." << endl;
		Bottle query;
		Bottle response;
		port.read(query,true);

		int type;
		int command = query.get(0).asVocab();

		switch ( command ) {


		case VOCAB_GO:
			success = query.size() >= 1;
			if (!success) { cout << "Invalid number of arguments." << endl; }
			success           = (success)  ?  mover.parseTrajBottle(query.get(1), thisTraj)  :  false;
			thisDistThreshold = (query.size() <= 2)  ?  distThreshold  :  query.get(2).asDouble();
			thisStepTimeout   = (query.size() <= 3)  ?  stepTimeout  :  query.get(3).asDouble();
			thisTrajTimeout   = (query.size() <= 4)  ?  trajTimeout  :  query.get(4).asDouble();
			success = success && mover.go(thisTraj, thisDistThreshold, finalDistThreshold, thisStepTimeout, thisTrajTimeout);
			if (success) {
				cout << "Trajectory successfully exectued." << endl;
				response.addVocab(VOCAB_OK);
			} else {
				cout << "Trajectory execution failed." << endl;
				response.addVocab(VOCAB_FAIL);
			}
			break;


		case VOCAB_SET:
			type = query.get(1).asVocab();
			switch ( type ) {
			case VOCAB_MASK:
				success = query.size() >= 2;
				if (!success) { cout << "Invalid number of arguments." << endl; }
				if (query.get(2).isList()) {
					success = (success)  ?  mover.setMask(query.get(2))  :  false;
				} else if (query.get(2).isString()) {
					if (query.get(2).asString() == "fingers") {
						if (success) { mover.maskFingers(); }
					} else if (query.get(2).asString() == "eyes") {
						if (success) { mover.maskEyes(); }
					} else if (query.get(2).asString() == "head") {
						if (success) { mover.maskHead(); }
					} else if (query.get(2).asString() == "off") {
						if (success) { mover.maskOff(); }
					} else {
						success = false;
						cout << "Unknown argument in \"set mask\"" << endl;
					}
				} else {
					success = false;
					cout << "Unknown argument type for \"set mask\"" << endl;
				}
				
				if (success) {
					cout << "New mask successfully set." << endl;
					response.addVocab(VOCAB_OK);
				} else {
					cout << "Set mask failed." << endl;
					response.addVocab(VOCAB_FAIL);
				}
				break;
				
			case VOCAB_DIST_THRESHOLD:
				success = query.size() >= 2;
				if (!success) { cout << "Invalid number of arguments." << endl; }
				success = success && (query.get(2).isInt() || query.get(2).isDouble());
				if (!success) { cout << "Distance threshold should be a number." << endl; }
				distThreshold = query.get(2).asDouble();
				if (success) { 
					cout << "New threshold set to " << distThreshold << endl;
					response.addVocab(VOCAB_OK);
				} else {
					cout << "Setting threshold failed." << endl;
					response.addVocab(VOCAB_FAIL);
				}
				break;
			case VOCAB_SPD:
				success = query.size() >= 2;
				if (!success) { cout << "Invalid number of arguments." << endl; }
				success = success && (query.get(2).isInt() || query.get(2).isDouble());
				if (!success) { cout << "Reference speed should be a number." << endl; }
				refSpd = query.get(2).asDouble();
				refSpd = max(0., min(refSpd, MAX_REFERENCE_SPEED));
				success = success && mover.setRefSpeed(refSpd);
				if (success) { 
					cout << "New reference speed set to " << refSpd << endl;
					response.addVocab(VOCAB_OK);
				} else {
					cout << "Sending reference speed failed." << endl;
					response.addVocab(VOCAB_FAIL);
				}
				break;
			case VOCAB_ACC:
				success = query.size() >= 2;
				if (!success) { cout << "Invalid number of arguments." << endl; }
				success = success && (query.get(2).isInt() || query.get(2).isDouble());
				if (!success) { cout << "Reference acceleration should be a number." << endl; }
				refAcc = query.get(2).asDouble();
				refAcc = max(0., min(refAcc, MAX_REFERENCE_ACCELERATION));
				success = success && mover.setRefAcceleration(refAcc);
				if (success) { 
					cout << "New reference acceleration set to " << refAcc << endl; 
					response.addVocab(VOCAB_OK);
				} else {
					cout << "Sending reference speed failed." << endl; 
					response.addVocab(VOCAB_FAIL);
				}
				break;
			case VOCAB_FWD:
				success = query.size() >= 2;
				if (!success) { cout << "Invalid number of arguments." << endl; }
				success = success && (query.get(2).isInt() || query.get(2).isDouble());
				if (!success) { cout << "Forward steps should be a number." << endl; }
				fwdSteps = query.get(2).asInt();
				fwdSteps = max(1, fwdSteps);
				success = success && mover.setFwdSteps(fwdSteps);
				if (success) { 
					cout << "New number of forward steps set to " << fwdSteps << endl;
					response.addVocab(VOCAB_OK);
				} else {
					cout << "Sending number of forward steps failed." << endl;
					response.addVocab(VOCAB_FAIL);
				}
				break;
			case VOCAB_MIN_TRAJ_TIME:
				success = query.size() >= 2;
				if (!success) { cout << "Invalid number of arguments." << endl; }
				success = success && (query.get(2).isInt() || query.get(2).isDouble());
				if (!success) { cout << "Minimum trajectory time should be a number." << endl; }
				minTrajTime = query.get(2).asDouble();
				minTrajTime = max(HARD_MIN_TRAJ_TIME, minTrajTime);
				success = success && mover.setMinTrajTime(minTrajTime);
				if (success) { 
					cout << "New minimum trajectory time set to " << minTrajTime << endl;
					response.addVocab(VOCAB_OK);
				} else {
					cout << "Sending minimum trajectory time failed." << endl;
					response.addVocab(VOCAB_FAIL);
				}
				break;
			case VOCAB_KP:
				success = query.size() >= 2;
				if (!success) { cout << "Invalid number of arguments." << endl; }
				success = success && (query.get(2).isInt() || query.get(2).isDouble());
				if (!success) { cout << "kp should be a number." << endl; }
				kp = query.get(2).asDouble();
				kp = max(0.0, kp); // no negative kp allowed
				success = success && mover.setKp(kp);
				if (success) { 
					cout << "New kp set to " << kp << endl;
					response.addVocab(VOCAB_OK);
				} else {
					cout << "Sending new kp failed." << endl;
					response.addVocab(VOCAB_FAIL);
				}
				break;
			case VOCAB_MIN_ABS_VEL:
				success = query.size() >= 2;
				if (!success) { cout << "Invalid number of arguments." << endl; }
				success = success && (query.get(2).isInt() || query.get(2).isDouble());
				if (!success) { cout << "Minimum velocity should be a number." << endl; }
				minabsvel = query.get(2).asDouble();
				minabsvel = min(max(0.0, minabsvel), HARD_MAX_MIN_ABS_VEL); // limit minabsvel between 0 and HARD_MAX_MIN_ABS_VEL
				success = success && mover.setMinAbsVel(minabsvel);
				if (success) { 
					cout << "New minimum velocity for bang-bang control set to " << kp << endl;
					response.addVocab(VOCAB_OK);
				} else {
					cout << "Setting minimum velocity for bang-bang control failed." << endl;
					response.addVocab(VOCAB_FAIL);
				}
				break;
			case VOCAB_MOVE_MODE:
				success = query.size() >= 2;
				if (!success) { cout << "Invalid number of arguments." << endl; }
				success = success && (query.get(2).isString() || query.get(2).isVocab());
				if (!success) { cout << "Move mode command invalid." << endl; }
				moveMode = query.get(2).asVocab();
				success = success && mover.setMode(moveMode);
				if (success) { 
					cout << "Move mode set to " << query.get(2).asString().c_str() << endl;
					response.addVocab(VOCAB_OK);
				} else {
					cout << "Move mode command invalid." << endl;
					response.addVocab(VOCAB_FAIL);
				}
				break;
			default:
				response.addString("Unknown set command. Type help for more information.");
				cout << "Unknown set command received." << endl;
				break;
			}
			break;
			

		case VOCAB_GET:
			type = query.get(1).asVocab();
			switch ( type ) {
			case VOCAB_MASK:
				mover.getMask(response);
				cout << "Replied with mask" << endl;
				break;
			case VOCAB_POSE:
				mover.getPose(response);
				cout << "Replied with pose" << endl;
				break;
			default:
				response.addString("Unknown get command. Type help for more information.");
				cout << "Unknown get command received." << endl;
				break;
			}
			break;

		case VOCAB_BLINK:
			mover.blink();
			response.addVocab(VOCAB_OK);
			cout << "Blinked." << endl;
			break;

		case VOCAB_STOP:
			mover.setStop();
			response.addVocab(VOCAB_OK);
			cout << "Stop flag set." << endl;
			break;

		case VOCAB_HELP:
			response.addVocab(Vocab::encode("many"));
			response.addString("iCub mover: \n");
			response.addString(	"go (POSE_1, ..., POSE_N) [DISTTHRESHOLD] [STEPTIMEOUT] [TRAJTIMEOUT]:\n\t"
								"execute trajectory.\n\t"
								"The position move command to the next pose is issued when both the RMS\n\t"
								"and the maximum absolute distance between the current and the requested\n\t"
								"pose is below DISTTHRESHOLD. When the robot is not within DISTTHRESHOLD\n\t"
								"within STEPTIMEOUT seconds, the next position move command is issued\n\t"
								"anyway. Poses are specified as: POSE = (PART_1, ..., PART_P) and PART =\n\t"
								"(JOINT_1, ..., JOINT_PJ), N = number of poses, P = number of parts and\n\t"
								"PJ = number of joints in each part: follow trajectory. Number of parts \n\t"
								"and number of joints in each part should match the bodyparts in the \n\t"
								"same order as specified in the \"parts\" option in the settings file or \n\t"
								"command line argument. Example for \"--parts head torso\": \n\t"
								"go ( ((0 0 0 0 0 0) (0 0 0)) ((-10 0 0 0 0 0) (10 0 0)) ).\n");
			response.addString(	"set mask (PART_1, ..., PART_P), with PART = (JOINT_1, ..., JOINT_PJ), P = \n\t"
								"number of joints in each part: set mask. Joints for which the mask is \n\t"
								"true will not be moved nor used during error computation when \n\t"
								"executing a trajectory. Number of parts and number of joints in each \n\t"
								"part should match the bodyparts in the same order as specified in \n\t"
								"the \"parts\" option in the settings file or command line argument. \n\t"
								"Example for \"--parts head torso\": mask ((0 0 0 1 1 1) (0 0 0)).\n");
			response.addString( "set mask fingers: mask finger joints in arm parts.\n");
			response.addString( "set mask eyes: mask eye joints in head part.\n");
			response.addString( "set mask off: disable mask for all joints in all parts.\n");
			response.addString( "set spd VALUE: set reference speed for all parts for all joints.\n");
			response.addString( "set acc VALUE: set reference acceleration for all parts for all joints.\n");
			response.addString( "set fwd VALUE: set number of forward steps.\n");
			response.addString( "set kp VALUE: set proportional term of P-controller.\n");
			response.addString( "set mav VALUE: set velocity below which bang-bang control is used.\n");
			response.addString( "set mint VALUE: set minimum trajectory time.\n");
			response.addString( "set mode pos: set position move mode.\n");
			response.addString( "set mode vel: set velocity move mode.\n");
			response.addString( "set thr VALUE: set distance threshold.\n");
			response.addString( "get mask: return current mask as a list of lists with the mask value \n\t"
								"of each joint for that part.\n");
			response.addString( "get pose: return current robot pose as a list of lists with the \n\t"
								"encoder position of each joint for that part.\n");
			response.addString( "blnk: blink with eyes.\n");
			response.addString( "stop: stop ongoing movement command\n");
			break;
		default:
			response.addString("Unknown command. Type help for more information.");
			cout << "Unknown command received." << endl;
			break;
		}

		port.reply(response);
	}
	return 0;
}