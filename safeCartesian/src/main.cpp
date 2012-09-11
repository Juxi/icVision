#include "cartesian.h"
#include <string>
#include <iostream>

#define VOCAB_SET  VOCAB3('s','e','t')
#define VOCAB_REFL VOCAB4('r','e','f','l')
#define VOCAB_WAYP VOCAB4('w','a','y','p')
#define VOCAB_HELP VOCAB4('h','e','l','p')

int main(int argc, char *argv[]) {
	Network yarp;
	Property command;
	command.fromCommand(argc, argv);
	Property file;
	Bottle settings;
	bool hasConfigFile = false;
	bool success = false;

	std::string fname;
	if (command.check("file")) {
		fname = command.find("file").asString();
		hasConfigFile = file.fromConfigFile(fname.c_str());
		settings = file.findGroup("SETTINGS");
	}

	// the following values are read from the configuration file, but can be overwritten from the command line
	std::string robot = "icubSim"; // default is simulator
	if ( settings.check("robot") )  { robot = settings.find("robot").asString().c_str(); }
	if ( command.check("robot") )  { robot = command.find("robot").asString().c_str(); }
	
	std::string portname = "/" + robot + "/" + "cartesian"; // the name of the RPC server
	if ( settings.check("port") )  { portname = settings.find("port").asString().c_str(); } 
	if ( command.check("port") )  { portname = command.find("port").asString().c_str(); } 

	double reflexTimeout = 10; // default reflex timeout in seconds
	if ( settings.check("reflexTimeout") )  { reflexTimeout = settings.find("reflexTimeout").asDouble(); }
	if ( command.check("reflexTimeout") )  { reflexTimeout = command.find("reflexTimeout").asDouble(); }

	double wpInterval = 2; // default waypoint interval in seconds
	if ( settings.check("wayPointInterval") )  { wpInterval = settings.find("wayPointInterval").asDouble(); }
	if ( command.check("wayPointInterval") )  { wpInterval = command.find("wayPointInterval").asDouble(); }

	std::string vSkinRpcPort = "/virtualSkin/filterRpc";
	if ( settings.check("vskinrpc") )  { vSkinRpcPort = settings.find("vskinrpc").asString().c_str(); }
	if ( command.check("vskinrpc") )  { vSkinRpcPort = command.find("vskinrpc").asString().c_str(); }

	std::string vSkinStatusPort = "/virtualSkin/filterStatus";
	if ( settings.check("vskinstatus") )  { vSkinStatusPort = settings.find("vskinstatus").asString().c_str(); }
	if ( command.check("vskinstatus") )  { vSkinStatusPort = command.find("vskinstatus").asString().c_str(); }

	// parse cartnames
	vector<string> cartnames;
	bool hasCarts = false;
	Bottle yCarts;
	if ( settings.check("cartnames") )  { yCarts = settings.findGroup("cartnames"); hasCarts = true; }
	if ( command.check("cartnames") )  { yCarts = command.findGroup("cartnames"); hasCarts = true; }
	if (!hasCarts) { cartnames.push_back("left_arm"); cartnames.push_back("right_arm"); } // default carts
	else { for (int i=1; i<yCarts.size(); i++) { cartnames.push_back(yCarts.get(i).toString().c_str()); } }

	// parse gaze names
	vector<string> gazenames;
	bool hasGazes = false;
	Bottle yGazes;
	if ( settings.check("gazenames") )  { yGazes = settings.findGroup("gazenames"); hasGazes = true; }
	if ( command.check("gazenames") )  { yGazes = command.findGroup("gazenames"); hasGazes = true; }
	if (!hasGazes) { gazenames.push_back("head"); } // default gazes
	else { for (int i=1; i<yGazes.size(); i++) { gazenames.push_back(yGazes.get(i).toString().c_str()); }	}
	

	// open cartesian and initialize
	Cartesian cartesian;
	if (!cartesian.init(robot, cartnames, gazenames)) { return 0; }
	cartesian.setReflexTimeout(reflexTimeout);
	cartesian.setWaypointInterval(wpInterval);

	// connect virtual skin
	if (!cartesian.connnectVSkin(robot, vSkinRpcPort, vSkinStatusPort))
	{ cout << "Error: Virtual Skin connection could not established! " << endl << endl; return 0; }

	// start thread
	cartesian.start();

	// open rpc port
	RpcServer port;
	port.open(portname.c_str());

	while (true) {
		cout << "Waiting for a message..." << endl;
		yarp::os::Bottle query;
		yarp::os::Bottle response;
		port.read(query,true);

		int type;
		int command = query.get(0).asVocab();

		switch ( command ) {


		case VOCAB_SET:
			type = query.get(1).asVocab();
			switch ( type ) {
			case VOCAB_REFL:
				success = query.size() >= 2;
				if (!success) { cout << "Invalid number of arguments." << endl; }
				success = success && (query.get(2).isInt() || query.get(2).isDouble());
				if (!success) { cout << "Reflex timeout should be a number." << endl; }
				reflexTimeout = query.get(2).asDouble();
				success = success && cartesian.setReflexTimeout(reflexTimeout);
				if (success) { 
					cout << "New reflex timeout set to " << reflexTimeout << endl;
					response.addString("OK");
				} else {
					cout << "Setting reflex timeout failed." << endl;
					response.addString("FAIL");
				}
				break;
			case VOCAB_WAYP:
				success = query.size() >= 2;
				if (!success) { cout << "Invalid number of arguments." << endl; }
				success = success && (query.get(2).isInt() || query.get(2).isDouble());
				if (!success) { cout << "Waypoint interval should be a number." << endl; }
				wpInterval = query.get(2).asDouble();
				success = success && cartesian.setWaypointInterval(wpInterval);
				if (success) { 
					cout << "New waypoint interval set to " << wpInterval << endl;
					response.addString("OK");
				} else {
					cout << "Setting waypoint interval failed." << endl;
					response.addString("FAIL");
				}
				break;
			default:
				response.addString("Unknown set command. Type help for more information.");
				break;
			}
			break;
		case VOCAB_HELP:
			response.addVocab(Vocab::encode("many"));
			response.addString("SafeCartesian: stops Cartesian interfaces when collision occurs, and waits for reflex.\n");
			response.addString(	"set refl VALUE: set reflex timeout in seconds\n\t");
			break;
		default:
			response.addString("Unknown command. Type help for more information.");
			break;
		}

		port.reply(response);
	}

	return 0;
}
