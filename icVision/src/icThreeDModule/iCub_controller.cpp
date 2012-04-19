#include "iCub_controller.h"
#include <iostream>	//for cout

using namespace yarp::sig;
using namespace yarp::os;

/**
*@brief    iCub control
*/
iCubController::iCubController() {
	simulation = false;
	
	head = new Part;
	head->initialized = false;
	
	torso = new Part;
	torso->initialized = false;
	
	yarp_network->init();
}

iCubController::iCubController(bool isSim) {
	simulation = isSim;
	
	head = new Part;
	head->initialized = false;
	torso = new Part;
	torso->initialized = false;
	
	yarp_network->init();
}

iCubController::~iCubController() {
	closeHead();
	yarp_network->fini();

	delete head;
	delete torso;
}

void iCubController::initializeRobot() {
	initHead();
	initTorso();

//	if( head->initialized && simulation) {
//		QVector<qreal> pose;
//		pose.append(-10.0);		pose.append(0.0);
//		pose.append(0.0); 		pose.append(0.0);
//		pose.append(0.0); 		pose.append(0.0);
//		if ( head->ctrl->positionMove( pose ) ) { printf("Head initialized!\n"); }		
//	}
//	if( torso->initialized && simulation ) {
//		QVector<qreal> pose;
//		pose.append(0.0); 		pose.append(0.0);
//		pose.append(35.0);
//		if ( torso->ctrl->positionMove( pose ) ) { printf("Torso initialized!\n"); }		
//	}
	
	
//	if(simulation) {
//		// clean up the world!
//	
//		RpcClient port;
//		port.open("/juxi/world-client");
//		if(! Network::connect("/juxi/world-client", "/icubSim/world") ) {
//			std::cout << std::endl << "ERROR: Could not connect to port: " << "/icubSim/world" << std::endl << std::endl;		
//			return;
//		}
//		
//		Bottle cmd, response;
//		
//		
//		// remove the blue (default) ball
//		cmd.addString("world"); cmd.addString("set");	
//		cmd.addString("ball");	cmd.addInt(1);
//		cmd.addDouble( -10.0); cmd.addDouble(-0.5); cmd.addDouble(-0.5);	
//		port.write(cmd, response);		
//		if( response.size() > 0) {
//			std::cout << "RPC Reply size (" << response.size() << "): " << response.toString() << std::endl;		
//		}
//		
//		cmd.clear(); response.clear();
//		// also red cube
//		cmd.addString("world"); cmd.addString("set");	
//		cmd.addString("cube");
//		cmd.addDouble( 0.0); cmd.addDouble(-1.5);
//		port.write(cmd, response);		
//		if( response.size() > 0) {
//			std::cout << "RPC Reply size (" << response.size() << "): " << response.toString() << std::endl;		
//		}
//		
//		Network::disconnect("/icubSim/world", "/juxi/world-client");
//		port.close();	
}
	
	

/**
 *@brief     initialises camera
 */
void iCubController::initHead() {
    if(!head->initialized) {	
		std::cout << " initialising head..." << std::endl;
		
//        head->port = BufferedPort<Bottle>();
		head->port = new Port();
        head->port->open("/juxi/head");
		if(simulation) Network::connect("/icubSim/head/state:o", "/juxi/head"); 
		else		   Network::connect("/icub/head/state:o", "/juxi/head"); 
		
//		head->port-

		head->initialized = true;
    }
}

void iCubController::closeHead() {
    if(head->initialized) {	
		std::cout<<"closing head ..."<<std::endl;

		if(simulation)  Network::disconnect("/icubSim/head/state:o", "/juxi/head");
		else			Network::disconnect("/icub/head/state:o", "/juxi/head");

        head->port->close();
        delete head->port;
		head->port = NULL;
		head->initialized = false;		
    }
}


void iCubController::initTorso() {
    if(!torso->initialized) {	
		std::cout<<"initialising torso..."<<std::endl;
		
		// state port, TODO needs cleanup ?
//        torso->port = new BufferedPort<Bottle>;
		torso->port = new Port();
        torso->port->open("/juxi/torso");
		if(simulation)  Network::connect("/icubSim/torso/state:o", "/juxi/torso"); 
		else			Network::connect("/icub/torso/state:o", "/juxi/torso"); 
		
		torso->initialized = true;
	}
}

void iCubController::closeTorso() {
    if(torso->initialized) {	
		std::cout<<"closing torso ..."<<std::endl;
			
		// state port
		if(simulation)  Network::disconnect("/icubSim/torso/state:o", "/juxi/torso");
		else			Network::disconnect("/icub/torso/state:o", "/juxi/torso");

        torso->port->close();
        delete torso->port;
		torso->port = NULL;
		torso->initialized = false;		
    }
}

//void iCubController::getWorldObjectPosition(const char *name, double &x, double &y, double &z) {
//	if(! simulation) return;
//
//	// read from simulation world interface
//	std::cout<<"read world rpc ..."<<std::endl;
//
//	RpcClient port;
//	port.open("/juxi/world-client");
//	if(! Network::connect("/juxi/world-client", "/icubSim/world") ) {
//		std::cout << std::endl << "ERROR: Could not connect to port: " << "/icubSim/world" << std::endl << std::endl;		
//		return;
//	}
//
//	Bottle cmd, response;
//	
//	// get red ball info
//	cmd.addString("world"); cmd.addString("get");
//	cmd.addString("ssph");	cmd.addInt(1);
//	port.write(cmd, response);
//	
//	if( response.size() < 3) { 
//		// no ball there yet? add one
//		cmd.clear(); response.clear();
//		
//		// create/make red ball
//		cmd.addString("world"); cmd.addString("mk");	
//		cmd.addString("ssph");	cmd.addDouble(0.005); // radius
//		x = 0.0; y = 0.53; z = 0.5;
//		cmd.addDouble( x ); cmd.addDouble( y ); cmd.addDouble( z );// pos
//		cmd.addInt(1); cmd.addInt(0); cmd.addInt(0); //red
//		port.write(cmd, response);
//		
//		std::cout << "RPC Reply size (" << response.size() << "): " << response.toString() << std::endl;				
//		
//	} else {
//		std::cout << "RPC Reply size (" << response.size() << "): " << response.toString() << std::endl;		
//		
//		x = response.get(0).asDouble();
//		y = response.get(1).asDouble();		
//		z = response.get(2).asDouble();
//	}
//		
//	
//	Network::disconnect("/icubSim/world", "/juxi/world-client");
//	port.close();	
//}

//void iCubController::setWorldObjectPosition(const char *name, double x, double y, double z) {
//	if(!simulation) return;
//
//	// set position of the object in the world
//	std::cout<<"set world position of " << name << " ..."<<std::endl;
//	
//	RpcClient port;
//	port.open("/juxi/world-client");
//	if(! Network::connect("/juxi/world-client", "/icubSim/world") ) {
//		std::cout << std::endl << "ERROR: Could not connect to port: " << "/icubSim/world" << std::endl << std::endl;		
//		return;
//	}
//	
//	Bottle cmd, response;
//	
//	cmd.addString("world"); cmd.addString("set");	
//	cmd.addString("ssph");	cmd.addInt(1);
//	cmd.addDouble( x ); cmd.addDouble( y ); cmd.addDouble( z );	
//	port.write(cmd, response);		
//	if( response.size() > 0) {
//		std::cout << "RPC Reply size (" << response.size() << "): " << response.toString() << std::endl;		
//	}
//		
//	Network::disconnect("/icubSim/world", "/juxi/world-client");
//	port.close();	
//
//}