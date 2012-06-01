// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdlib>
#include <iostream>
#include "icFilterPort.h"

using namespace yarp::os;
using namespace yarp::sig;

icFilterPort::icFilterPort(int ID_in) {
	moduleID = ID_in;
	sprintf(port_name, "%s/%05d:i", "/icVision/filterIn", moduleID);
}

icFilterPort::~icFilterPort() { 
	
}

bool icFilterPort::openAndConnectPort(const char *in_port_name) {
	if(! this->open( port_name )){
		cout << getName() << ": Unable to open port " << port_name << endl;		
		return false;
	}
		
	printf("Trying to connect to %s\n", in_port_name);
	if(! Network::connect(in_port_name, port_name) ) {
		cout << getName() << ": Unable to connect to port " << in_port_name << " with " << port_name << endl;
		return false;
	}	
	return true;
}

