// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include "saliency_module.h"
#include "saliency.h"


SaliencyModule::SaliencyModule() {
	//moduleName = "IM-CLeVeR::Vision::Saliency";
	moduleName = "saliency";
	saliency = new SaliencyWorker();
}


SaliencyModule::~SaliencyModule() { 
	delete saliency;
}

double SaliencyModule::getPeriod()
{
   return 1;
   //module periodicity (seconds)
}

/*
* This is our main function. Will be called periodically every getPeriod() seconds.
*/
bool SaliencyModule::updateModule()
{
	//std::cout << "next step... " <<std::endl;

	//Use YARP port to get images
	if ( imageInLeft.getInputCount() > 0 &&
		 imageInRight.getInputCount() > 0	){
		// std::cout << "There are images available from the left cam... " << std::endl;
		iCubLeft = imageInLeft.read();  // read an image		
		imageLeft = cv::Mat((IplImage*)iCubLeft->getIplImage(),false);
		iCubRight = imageInRight.read();  // read an image		
		imageRight = cv::Mat((IplImage*)iCubRight->getIplImage(),false);

		// TODO
		// do something :)
		//outputImageLeft = imageLeft.clone();
		IplImage **h = saliency->getSaliencyMap((IplImage*)iCubLeft->getIplImage(), (IplImage*)iCubRight->getIplImage());
		
		outputImages[0] = h[0];
		outputImages[1] = h[1];

		if(!outputImages[0].empty()) {
			//Send output images ... just place to outputImageLeft or outputImageRight what do you want to see
			yarp::sig::ImageOf<yarp::sig::PixelBgr> &imageLeftTmp = imageOutLeft.prepare();
			
			imageLeftTmp.resize(outputImages[0].cols,outputImages[0].rows);

			IplImage image_iplLeftimage = (IplImage) outputImages[0];

			cvCopy( &image_iplLeftimage, (IplImage *) imageLeftTmp.getIplImage());
			
			imageOutLeft.write();
		} else std::cout << "empty image... ?! " << std::endl;
    }

/*
        //Send object position on the port
        Bottle output_message,answer;
        ostringstream buffer2send;
//        output_message.clear();
//        answer.clear();
//        buffer2send.clear();
//        flush(buffer2send);

        string command2send = object_list[i].getSimCommand(POSE, isICubSim);
        cout<<"Command transmitted: "<<endl;
        cout<<command2send<<endl;
        buffer2send<<command2send;
        output_message.fromString(buffer2send.str().c_str());
        moduleOutput.write(output_message, answer);
        cout<<"Answer : "<<answer.toString()<<endl;

//        output_message.clear();
//        answer.clear();
//        flush(buffer2send);
//
//        cout<<"Command transmitted: "<<endl;
//        cout<<object_list[i].getSimCommand(ORIENTATION, isICubSim)<<endl;
//        buffer2send<<object_list[i].getSimCommand(ORIENTATION, isICubSim);
//        output_message.fromString(buffer2send.str().c_str());
//        moduleOutput.write(output_message, answer);
//        cout<<"Answer : "<<answer.toString()<<endl;

*/
   return true;
}


/*
* Message handler. Just echo all received messages.
*/
bool SaliencyModule::respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply)
{
	cout << "Message Received: (echo is on)" << endl;
	if (command.get(0).asString() == "quit" || command.get(0).asString()=="exit"){
		close();
		return false;
	} else {
		reply.clear();
		reply.addString("The command is not valid");
	}
	return true;
}

/*
* Interrupt function.
*/
bool SaliencyModule::interruptModule()
{
   std::cout << "Interrupt of the module received, waiting for clean up! " << std::endl;
   imageInLeft.interrupt();
   handlerPort.interrupt();

   return true;
}

/*
* Configure function. Receive a previously initialized
* resource finder object. Use it to configure your module.
* Open port and attach it to message handler.
*/
//bool SaliencyModule::configure(yarp::os::ResourceFinder &rf)
bool SaliencyModule::configure(yarp::os::Searchable& config)
{
 /* Process all parameters from both command-line and .ini file */
   //moduleName = (rf.find("moduleName")).toString();

  /*
   * attach a port of the same name as the module (prefixed with a /) to the module
   * so that messages received from the port are redirected to the respond method
   */

	handlerPortName =  "/";
	handlerPortName += getName();         // use getName() rather than a literal

	if (! handlerPort.open(handlerPortName.c_str())) {
		cout << getName() << ": Unable to open port " << handlerPortName << endl;
		return false;
	}

	attach(handlerPort);

	//set-up input/output ports
	inputPortNameLeft = "/" + moduleName + "/left/image:i";
	if(! imageInLeft.open( inputPortNameLeft.c_str() )){
		return false;
	}

	outputPortNameLeft = "/" + moduleName + "/left/image:o";
	if(! imageOutLeft.open( outputPortNameLeft.c_str() )){
		return false;
	}

    outputPortName = "/" + moduleName + "/output:o";
	if(! moduleOutput.open( outputPortName.c_str() )){
		return false;
	}
	
	inputPortNameRight = "/" + moduleName + "/right/image:i";
	if(! imageInRight.open( inputPortNameRight.c_str() )){
		return false;
	}

	outputPortNameRight = "/" + moduleName + "/right/image:o";
	if(! imageOutRight.open( outputPortNameRight.c_str() )){
		return false;
	}

  	

	return true ;      // let the RFModule know everything went well
}



bool SaliencyModule::open(yarp::os::Searchable& config)
{
	if( ! configure(config) ) return false;

	std::cout << "Starting the IM-CLeVeR Saliency module" << std::endl;

	return true;
}


/*
* Close function, to perform cleanup.
*/
bool SaliencyModule::close()
{
	std::cout << "Closing and cleaning up the module!" << std::endl;
	imageInLeft.close();
	handlerPort.close();

	return true;
}

