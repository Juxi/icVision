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
		std::cout<<endl<<"initialising left camera ..."<<std::endl;
		
        left_camera->port = new BufferedPort<ImageOf<PixelRgb> >;
        left_camera->port->open("/juxi/cam/left");
		
		yarp_network->connect("/icubSim/cam/left", "/juxi/cam/left");	
        left_camera->last_image = left_camera->port->read();
        left_camera->initialized = true;
    }
}

/**
 *@brief     initialises right camera
 */
void iCubController::initRightCamera() {
    if(!right_camera->initialized) {	
		std::cout<<endl<<"initialising right camera ..."<<std::endl;
		
        right_camera->port = new BufferedPort<ImageOf<PixelRgb> >;
        right_camera->port->open("/juxi/cam/right");
		
		yarp_network->connect("/icubSim/cam/right", "/juxi/cam/right");	
        right_camera->last_image = right_camera->port->read();
        right_camera->initialized = true;
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
		std::cout<<endl<<"closing left camera ..."<<std::endl;
		
		yarp_network->disconnect("/icubSim/cam/left", "/juxi/cam/left");	
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
		
		yarp_network->disconnect("/icubSim/cam/right", "/juxi/cam/right");
        right_camera->port->close();
        delete right_camera->port;
		right_camera->port = NULL;
		right_camera->initialized = false;		
    }
}

void iCubController::initHead() {
    if(!head->initialized) {	
		std::cout<<endl<<"initialising head..."<<std::endl;
		
        head->port = new Port;
        head->port->open("/juxi/head");
		
//		yarp_network->connect("/icubSim/head/status:o", "/juxi/head");	
		Network::connect("/icubSim/head/state:o", "/juxi/head"); 

        head->initialized = true;
    }
}

void iCubController::closeHead() {
    if(head->initialized) {	
		std::cout<<endl<<"closing head ..."<<std::endl;
		
		Network::disconnect("/icubSim/head/state:o", "/juxi/head");
        head->port->close();
        delete head->port;
		head->port = NULL;
		head->initialized = false;		
    }
}

void iCubController::initTorso() {
    if(!torso->initialized) {	
		std::cout<<endl<<"initialising torso..."<<std::endl;
		
		// state port
        torso->port = new Port;
        torso->port->open("/juxi/torso");
		Network::connect("/icubSim/torso/state:o", "/juxi/torso"); 
		
        torso->initialized = true;
    }
}

void iCubController::closeTorso() {
    if(torso->initialized) {	
		std::cout<<endl<<"closing torso ..."<<std::endl;
		// state port
		Network::disconnect("/icubSim/torso/state:o", "/juxi/torso");
        torso->port->close();
        delete torso->port;
		torso->port = NULL;
		torso->initialized = false;		
    }
}