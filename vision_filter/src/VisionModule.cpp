/*
 *  VisionModule.cpp
 *  Main class for the vision filter yarp module 
 *
 *  Copyright: Juxi Leitner <juxi.leitner@gmail.com>, 2011
 */

#include "VisionModule.h"
#include "GpImage.h"

using namespace std;


VisionFilterModule::VisionFilterModule() {
}

VisionFilterModule::~VisionFilterModule() {
}

double VisionFilterModule::getPeriod() {
	return 1.0; //module periodicity (seconds)
}

bool VisionFilterModule::updateModule() {

	//Use YARP port to get images
	getImageLeft =  ( imageInLeft.getInputCount() > 0 );
	getImageRight = ( imageInRight.getInputCount() > 0 );

	if ( getImageLeft + getImageRight < 1) {
		Time::delay(0.1);
		cout << "port not available ---- " << getImageLeft << "/" << getImageRight << endl;
	}	else {

		iCub_leftImg = imageInLeft.read();  // read an image
		iCub_rightImg = imageInRight.read();  // read an image

		imageLeft = Mat((IplImage*)iCub_leftImg->getIplImage(),false);
		imageRight = Mat((IplImage*)iCub_rightImg->getIplImage(),false);
		
		// do stuff with opencv
		
		GpImage bla;
		
		

		std::cout << "Done!" << endl;
	}

	return true;

}

bool VisionFilterModule::respond(const Bottle& command, Bottle& reply){

  cout<<"Got something, echo is on"<<endl;
     if (command.get(0).asString()=="quit" || command.get(0).asString()=="stop"){
         close();
         return false;
     }
     else{

       reply.clear();
       reply.addString("The command is not valid");

     }
      // reply=output_message; //command;
     return true;

}

bool VisionFilterModule::configure(yarp::os::ResourceFinder &rf){

    /* Process all parameters from both command-line and .ini file */
    moduleName   = "VisionFilter"; //(rf.find("moduleName")).toString();

    /*
     * attach a port of the same name as the module (prefixed with a /) to the module
     * so that messages received from the port are redirected to the respond method
     */

	handlerPortName =  "/";
	handlerPortName += getName();         // use getName() rather than a literal

    if (!handlerPort.open(handlerPortName.c_str())) {
       cout << getName() << ": Unable to open port " << handlerPortName << endl;
       return false;
    }

    attach(handlerPort);                  // attach to port

    //set-up input/output ports
    inputPortNameLeft = "/" + moduleName + "/left/image:i";
    if(! imageInLeft.open( inputPortNameLeft.c_str() )){
      return false;
    }

    inputPortNameRight = "/" + moduleName + "/right/image:i";
    if(! imageInRight.open( inputPortNameRight.c_str() )){
      return false;
    }

	if(	Network::connect("/icub/cam/left", inputPortNameLeft.c_str()) &&
	    Network::connect("/icub/cam/right", inputPortNameRight.c_str())) {
		cout << "Connected to the iCub Cameras" << endl;
		return true;
	}else if( Network::connect("/icubSim/cam/left", inputPortNameLeft.c_str()) &&
			  Network::connect("/icubSim/cam/right", inputPortNameRight.c_str())) {
		cout << "Connected to the iCub _!SIMULATOR!_ Cameras" << endl;		
		return true;
	} else {
		cout << getName() << ": Unable to connect ports to either the iCub or the Sim" << endl;
	}
	return false;

}

bool VisionFilterModule::interruptModule(){
  cout<<"Interrupting your module, for port cleanup"<<endl;
  imageInLeft.interrupt();
  imageInRight.interrupt();
  handlerPort.interrupt();

  return true;
}

bool VisionFilterModule::open(Searchable& config){ //remember to set the object up.
  //TODO change the config setting for customizable configuration (ref Taio code)

    /* prepare and configure the resource finder */
     ResourceFinder rf;
     rf.setVerbose( true );
     rf.setDefaultConfigFile( "VisionFilter.ini" ); //overridden by --from parameter
     rf.setDefaultContext( "VisionFilter/conf" );   //overridden by --context parameter
     rf.configure( "ICUB_ROOT", 0, NULL );
     //rf.configure( "../home/migliore/Dropbox/Projects/YarpObjectLocalization/bin/", 0, NULL );

     cout << "Starting module ..." << endl;

     if( ! configure(rf) ){
       return false;
     }
     return true;
}

bool VisionFilterModule::close() {
	std::cout << "Calling close function" << endl;
	imageInLeft.close();
	imageInRight.close();
	handlerPort.close();
	return true;
}
