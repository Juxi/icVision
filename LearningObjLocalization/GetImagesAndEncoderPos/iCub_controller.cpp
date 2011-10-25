#include "iCub_controller.h"
#include <iostream>	//for cout

using namespace yarp::sig;
using namespace yarp::os;

/**
*@brief    iCub control
*/
iCubController::iCubController() {
	simulation = true;
	
	left_camera = new Camera;	
	right_camera = new Camera;
	
	left_camera->initialized = false;
	right_camera->initialized = false;
	
	left_camera_port_name  = "/juxi/cam/left";
	right_camera_port_name = "/juxi/cam/right";
	
	head = new Part;
	head->initialized = false;
	
	torso = new Part;
	torso->initialized = false;
	
	yarp_network->init();
}

iCubController::iCubController(bool isSim) {
	simulation = isSim;
	
	left_camera = new Camera;	
	right_camera = new Camera;

	left_camera->initialized = false;
	right_camera->initialized = false;

	left_camera_port_name  = "/juxi/cam/left";
	right_camera_port_name = "/juxi/cam/right";
	
	head = new Part;
	head->initialized = false;

	torso = new Part;
	torso->initialized = false;
	
	yarp_network->init();
}

iCubController::~iCubController() {
	closeHead();
	closeCameras();
	yarp_network->fini();

	delete head;
	delete torso;
	delete left_camera;
	delete right_camera;
}

void iCubController::toggleConnection() {
	((QPushButton*) QObject::sender())->setEnabled(false);
	((QPushButton*) QObject::sender())->setText("Connecting...");
	
	initCameras();
	initHead();
	initTorso();
}

void iCubController::initializeRobot() {
	if( head->initialized && simulation) {
		QVector<qreal> pose;
		pose.append(-10.0);		pose.append(0.0);
		pose.append(0.0); 		pose.append(0.0);
		pose.append(0.0); 		pose.append(0.0);
		if ( head->ctrl->positionMove( pose ) ) { printf("Head initialized!\n"); }		
	}
	if( torso->initialized && simulation ) {
		QVector<qreal> pose;
		pose.append(0.0); 		pose.append(0.0);
		pose.append(35.0);
		if ( torso->ctrl->positionMove( pose ) ) { printf("Torso initialized!\n"); }		
	}
	
	
	if(simulation) {
		// clean up the world!
	
		RpcClient port;
		port.open("/juxi/world-client");
		if(! Network::connect("/juxi/world-client", "/icubSim/world") ) {
			std::cout << std::endl << "ERROR: Could not connect to port: " << "/icubSim/world" << std::endl << std::endl;		
			return;
		}
		
		Bottle cmd, response;
		
		
		// remove the blue (default) ball
		cmd.addString("world"); cmd.addString("set");	
		cmd.addString("ball");	cmd.addInt(1);
		cmd.addDouble( -10.0); cmd.addDouble(-0.5); cmd.addDouble(-0.5);	
		port.write(cmd, response);		
		if( response.size() > 0) {
			std::cout << "RPC Reply size (" << response.size() << "): " << response.toString() << std::endl;		
		}
		
		cmd.clear(); response.clear();
		// also red cube
		cmd.addString("world"); cmd.addString("set");	
		cmd.addString("cube");
		cmd.addDouble( 0.0); cmd.addDouble(-1.5);
		port.write(cmd, response);		
		if( response.size() > 0) {
			std::cout << "RPC Reply size (" << response.size() << "): " << response.toString() << std::endl;		
		}
		
		Network::disconnect("/icubSim/world", "/juxi/world-client");
		port.close();	
	}
		
}
	
	

/**
 *@brief     initialises camera
 */
void iCubController::initCameras() {
	if(! (left_camera->initialized && right_camera->initialized)) {		
		initLeftCamera();
		initRightCamera();
	}
	emit connectionStatus(left_camera->initialized && right_camera->initialized);		
}

void iCubController::initLeftCamera() {
    if(!left_camera->initialized) {	
		std::cout << std::endl << "initialising left camera ..." << simulation;
		
        left_camera->port = new BufferedPort<ImageOf<PixelRgb> >;
        left_camera->port->open("/juxi/cam/left");
		
		if(simulation)	yarp_network->connect("/icubSim/cam/left", "/juxi/cam/left");	
		else			yarp_network->connect("/icub/cam/left", "/juxi/cam/left");	
        left_camera->last_image = left_camera->port->read();
        left_camera->initialized = true;
		
		std::cout << "done!" << std::endl;
    }
}

/**
 *@brief     initialises right camera
 */
void iCubController::initRightCamera() {
    if(!right_camera->initialized) {	
		std::cout<<endl<<"initialising right camera ..." << simulation;
		
        right_camera->port = new BufferedPort<ImageOf<PixelRgb> >;
        right_camera->port->open("/juxi/cam/right");
		
		if(simulation)	yarp_network->connect("/icubSim/cam/right", "/juxi/cam/right");	
		else			yarp_network->connect("/icub/cam/right", "/juxi/cam/right");	
        right_camera->last_image = right_camera->port->read();
        right_camera->initialized = true;
		
		std::cout << "done!" << std::endl;
    }
}

/**
 *@brief     initialises camera
 */
void iCubController::closeCameras() {
	if(left_camera->initialized && right_camera->initialized) {
		closeLeftCamera();	
		closeRightCamera();
		emit connectionStatus(left_camera->initialized && right_camera->initialized);		
	}
}

void iCubController::closeLeftCamera() {
    if(left_camera->initialized) {	
		std::cout<<endl<<"closing left camera ..." << std::endl;

		if(simulation)  yarp_network->disconnect("/icubSim/cam/left", "/juxi/cam/left");	
		else			yarp_network->disconnect("/icub/cam/left", "/juxi/cam/left");	
        left_camera->port->close();
        delete left_camera->port;
		left_camera->port = NULL;		
        left_camera->initialized = false;		
    }
}

/**
 *@brief     initialises right camera
 */
void iCubController::closeRightCamera() {
    if(right_camera->initialized) {	
		std::cout<<endl<<"closing right camera ..."<<std::endl;
		
		if(simulation)  yarp_network->disconnect("/icubSim/cam/right", "/juxi/cam/right");
		else			yarp_network->disconnect("/icub/cam/right", "/juxi/cam/right");

        right_camera->port->close();
        delete right_camera->port;
		right_camera->port = NULL;
		right_camera->initialized = false;		
    }
}




/**
 *@brief     initialises camera
 */
void iCubController::initHead() {
    if(!head->initialized) {	
		std::cout<<endl<<"initialising head..."<<std::endl;
		
		// needs cleanup , TODO
        head->port = new Port;
        head->port->open("/juxi/head");
		if(simulation) Network::connect("/icubSim/head/state:o", "/juxi/head"); 
		else		   Network::connect("/icub/head/state:o", "/juxi/head"); 

		// needs cleanup , TODO, might not need both!
		head->ctrl = new PartController();
		if(simulation) head->ctrl->open("icubSim", "head");
		else head->ctrl->open("icub", "head");
		if( head->ctrl->isValid() ) head->initialized = true;
    }
}

void iCubController::closeHead() {
    if(head->initialized) {	
		std::cout<<endl<<"closing head ..."<<std::endl;

		if( head->ctrl ) {
			head->ctrl->stop();
			head->ctrl->close();
			head->ctrl = NULL;
		}

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
		std::cout<<endl<<"initialising torso..."<<std::endl;
		
		// state port, TODO needs cleanup ?
        torso->port = new Port;
        torso->port->open("/juxi/torso");
		if(simulation)  Network::connect("/icubSim/torso/state:o", "/juxi/torso"); 
		else			Network::connect("/icub/torso/state:o", "/juxi/torso"); 
		
		// needs cleanup , TODO, might not need both!
		torso->ctrl = new PartController();
		if(simulation) torso->ctrl->open("icubSim", "torso");
		else torso->ctrl->open("icub", "torso");
		if( torso->ctrl->isValid() ) torso->initialized = true;
	}
}

void iCubController::closeTorso() {
    if(torso->initialized) {	
		std::cout<<endl<<"closing torso ..."<<std::endl;
		
		if( torso->ctrl ) {
			torso->ctrl->stop();
			torso->ctrl->close();
			torso->ctrl = NULL;
		}
		
		// state port
		if(simulation)  Network::disconnect("/icubSim/torso/state:o", "/juxi/torso");
		else			Network::disconnect("/icubSim/torso/state:o", "/juxi/torso");

        torso->port->close();
        delete torso->port;
		torso->port = NULL;
		torso->initialized = false;		
    }
}

void iCubController::getWorldObjectPosition(const char *name, double &x, double &y, double &z) {
	if(! simulation) return;

	// read from simulation world interface
	std::cout<<"read world rpc ..."<<std::endl;

	RpcClient port;
	port.open("/juxi/world-client");
	if(! Network::connect("/juxi/world-client", "/icubSim/world") ) {
		std::cout << std::endl << "ERROR: Could not connect to port: " << "/icubSim/world" << std::endl << std::endl;		
		return;
	}

	Bottle cmd, response;
	
	// get red ball info
	cmd.addString("world"); cmd.addString("get");
	cmd.addString("ssph");	cmd.addInt(1);
	port.write(cmd, response);
	
	if( response.size() < 3) { 
		// no ball there yet? add one
		cmd.clear(); response.clear();
		
		// create/make red ball
		cmd.addString("world"); cmd.addString("mk");	
		cmd.addString("ssph");	cmd.addDouble(0.005); // radius
		x = 0.0; y = 0.53; z = 0.5;
		cmd.addDouble( x ); cmd.addDouble( y ); cmd.addDouble( z );// pos
		cmd.addInt(1); cmd.addInt(0); cmd.addInt(0); //red
		port.write(cmd, response);
		
		std::cout << "RPC Reply size (" << response.size() << "): " << response.toString() << std::endl;				
		
	} else {
		std::cout << "RPC Reply size (" << response.size() << "): " << response.toString() << std::endl;		
		
		x = response.get(0).asDouble();
		y = response.get(1).asDouble();		
		z = response.get(2).asDouble();
	}
		
	
	Network::disconnect("/icubSim/world", "/juxi/world-client");
	port.close();	
}

void iCubController::setWorldObjectPosition(const char *name, double x, double y, double z) {
	if(!simulation) return;

	// set position of the object in the world
	std::cout<<"set world position of " << name << " ..."<<std::endl;
	
	RpcClient port;
	port.open("/juxi/world-client");
	if(! Network::connect("/juxi/world-client", "/icubSim/world") ) {
		std::cout << std::endl << "ERROR: Could not connect to port: " << "/icubSim/world" << std::endl << std::endl;		
		return;
	}
	
	Bottle cmd, response;
	
	cmd.addString("world"); cmd.addString("set");	
	cmd.addString("ssph");	cmd.addInt(1);
	cmd.addDouble( x ); cmd.addDouble( y ); cmd.addDouble( z );	
	port.write(cmd, response);		
	if( response.size() > 0) {
		std::cout << "RPC Reply size (" << response.size() << "): " << response.toString() << std::endl;		
	}
		
	Network::disconnect("/icubSim/world", "/juxi/world-client");
	port.close();	

}