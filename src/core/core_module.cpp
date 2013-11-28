// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include <cstdlib>
#include <iostream>
#include "core_module.h"
#include "../icFilterModule/icImage.h"

#include "cv.h"


using namespace yarp::os;
using namespace yarp::sig;

CoreModule::CoreModule() : isRunning (true) {
	//moduleName = "IM-CLeVeR::Vision::Reaching";
	setName("icVision");
	nextModuleID = 0;
}

CoreModule::~CoreModule() { 
	if( outputImageToWrite != NULL )
		cvReleaseImage(&outputImageToWrite);
}


bool CoreModule::open(yarp::os::Searchable& config) {
	if( ! configure(config) ) return false;
	return true;
}

//void CoreModule::runModule(int argc, char *argv[])
//{	
//	Property options;
//	options.fromCommand(argc,argv,true);
//		
////	options.setMonitor(&HELPER(implementation));
//	// check if we're being asked to read the options from file
//	Value *val;
//	if (options.check("file",val,"configuration file to use, if any")) {
//		ConstString fname = val->toString();
//		options.unput("file");
////		ACE_OS::printf("Working with config file %s\n", fname.c_str());
//		options.fromConfigFile(fname,false);
//		
//		// interpret command line options as a set of flags again
//		// (just in case we need to override something)
//		options.fromCommand(argc,argv,true,false);
//	}
//			
//	name = options.check("name",Value(name),"name of module").asString();
//		
//	if( configure(options) ) {
//		updateModule();
//	}
//}

/*
 * Configure function. Receive a previously initialized
 * resource finder object. Use it to configure your module.
 * Open port and attach it to message handler.
 */
bool CoreModule::configure(yarp::os::Searchable& config)
{
	
	/* Process all parameters from both command-line and .ini file */
	robotName = (config.find("robot")).toString();
	if( robotName.empty() ) {
		robotName = "icubSim";
		std::cout << "WARNING! No robot name found using " << robotName << std::endl;
	}
	
	/*
	 * attach a port of the same name as the module (prefixed with a /) to the module
	 * so that messages received from the port are redirected to the respond method
	 */
	
	rpcPortName =  "/";
	rpcPortName += getName();	// use getName() rather than a literal
	
	
	string inputPortName, serverPortName;
			
	// TODO do not need both for now
	inputPortName = "/";		serverPortName = "/";
	inputPortName += getName(); serverPortName += robotName;
	inputPortName += "/left";	serverPortName += "/cam/left";

	// trying to connect to the left camera
	if(! leftInPort.open( inputPortName.c_str() )){
		cout << getName() << ": Unable to open port " << inputPortName << endl;
		return false;
	}
	printf("Trying to connect to %s\n", serverPortName.c_str());
	if(! yarp.connect(serverPortName.c_str(), inputPortName.c_str()) ) {
		cout << getName() << ": Unable to connect to port " << serverPortName.c_str() << " with " << inputPortName.c_str() << endl;
		return false;
	}

//	// trying to connect to the right camera
//	inputPortName = "/";		serverPortName = "/";
//	inputPortName += getName(); serverPortName += robotName;
//	inputPortName += "/right";	serverPortName += "/cam/right";
//	if(! rightInPort.open( inputPortName.c_str() )){
//		cout << getName() << ": Unable to open port " << inputPortName << endl;		
//		return false;
//	}
//	
//	printf("Trying to connect to %s\n", inputPortName.c_str());
//	if(! yarp.connect(serverPortName.c_str(), inputPortName.c_str()) ) {
//		cout << getName() << ": Unable to connect to port " << serverPortName.c_str() << " with " << inputPortName.c_str() << endl;
//		return false;
//	}
	
	
	rpcPortName = "/";
	rpcPortName += getName();
	rpcPortName += "/rpc:i";
	
	if(! rpcPort.open( rpcPortName.c_str() )){
		return false;
	}
	

	// output (merged outputs from the other filters!)
	outputPortName  = "/";
	outputPortName += getName();
	outputPortName += "/output:o";
	if(! outputPort.open( outputPortName.c_str() )){
		return false;
	}
	
	// start RPC thread
	rpcThread = new ModuleRPCThread(this);
	rpcThread->start();	

	// TODO add this
	// attachTerminal();
	// attach((Port &)rpcPort);
	return true;      // let the RFModule know everything went well
}


void CoreModule::printDebug(const char* str)
{
	printf("[icVision::Core] %s\n", str);
}



double CoreModule::getPeriod() {
	return 0.1;
}



// debug output
//	char buf[99];
//	sprintf(buf, "%zu filter outputs were received.", filterOutputs.size());
//	printDebug(buf);	

/*
* This is our main function. Will be called periodically every getPeriod() seconds.
*/
bool CoreModule::updateModule()
{
	bool wewantoverlay = true;
	// get input images (from eyes?)

	IplImage* in = NULL;
	
	ImageOf<PixelBgr> *left_image = NULL;
//	ImageOf<PixelBgr> *right_image = NULL;	
	
//	if( isReadingFileFromHDD ) {
//		in = cvLoadImage(fileName.c_str());
//	} else {	
		// read image from the port
		left_image = leftInPort.read();  // read an image
		if (left_image == NULL) { 
			std::cout << "ERROR: Could not read from port '" << leftInPort.getName() << "'!" << std::endl;
			return false;
		}	
		in = (IplImage*) left_image->getIplImage();
	
	// IplImage* rgb = cvLoadImage("input.png");
	
	// maybe convert to gray?
	int ImageWidth = in->width;
	int ImageHeight = in->height;	

	IplImage* out8 = cvCreateImage(cvSize(ImageWidth, ImageHeight), IPL_DEPTH_8U, 3);
	
	// convert the of filtered image to be outputted
	cvConvertScale(in, out8, 1.0, 0.0);
	icImage *outputImg = new icImage(out8);	
	
	// get all filter output images...
	std::vector<icImage*> filterOutputs;
	icImage *rawoutputImg = NULL;	
	
	std::vector<ModuleInfo>::iterator itr;
	for ( itr = listOfModules.begin(); itr != listOfModules.end(); ++itr ) {

		// check if we want this filters output in the output of the core
		if( itr->isUsedInOutput ) {
			std::string sName = itr->shortName;
			icImage *in = (*itr).port->getImage();
			if(!in) continue;

			IplImage* single = cvCreateImage(cvSize(ImageWidth, ImageHeight), IPL_DEPTH_8U, 1);
			cvCvtColor(in->Image, single, CV_BGR2GRAY);
			icImage *current = new icImage(single);
			
			if(rawoutputImg == NULL) {
				rawoutputImg = current->Clone();
			} else {
				icImage* prev = rawoutputImg;
				rawoutputImg = prev->add(current);	//absdiff
				delete prev;
			}
			
			// paint on outputImg
			drawBoxesAndCaption(outputImg, current, sName);	
			
//			// debug (save image)
//			std::string s = (*itr).name + "-";
//			char c[2];
//			c[0] = '0' + (*itr).ID;
//			c[1] = '\0';
//			s.append(c);
//			s.append(".png");
//			current->Save(s.c_str());
//			s = "Saved: " + s;
//			printDebug(s.c_str());

			delete current;
		}
	}

	
	if(outputImg) {
		if(wewantoverlay && rawoutputImg) {
			// overlay original		// IPL_DEPTH_32F or IPL_DEPTH_8U
			IplImage* r = cvCreateImage(cvSize(ImageWidth, ImageHeight), IPL_DEPTH_8U, 1);
			IplImage* g = cvCreateImage(cvSize(ImageWidth, ImageHeight), IPL_DEPTH_8U, 1);
			IplImage* b = cvCreateImage(cvSize(ImageWidth, ImageHeight), IPL_DEPTH_8U, 1);
			
			// cvCvtColor(gray, rgb, CV_GRAY2RGB);
			cvSplit(outputImg->Image, b, g, r, NULL);
			cvAdd(r, rawoutputImg->Image, r);
			cvMerge(b, g, r, NULL, outputImg->Image);	

//			delete outputImg;
//			outputImg = new icImage(rgb);
			
			cvReleaseImage(&r);
			cvReleaseImage(&g);	
			cvReleaseImage(&b);	
		}
		
		
//		if(outputImageToWrite == NULL)
//			outputImageToWrite = cvCreateImage(cvSize(ImageWidth, ImageHeight), IPL_DEPTH_8U, 3);
//		
//		cvCopy(outputImg->Image, outputImageToWrite);
		
		ImageOf<PixelBgr>& output = outputPort.prepare();
		output.wrapIplImage(outputImg->Image); 
		outputPort.write();

		
		//if(isReadingFileFromHDD)
		//	cvSaveImage("output.png", outputImageToWrite);
			//outputImg->Save("output.png");		
		
		delete outputImg;
		// does also 	cvReleaseImage(&out8); hopefully
	}
	if(rawoutputImg) {
		rawoutputImg->Save("rawoutput.png");	
		delete rawoutputImg;
	}
	
	
	return true;
}

//////////////////
//
void CoreModule::drawBoxesAndCaption(icImage *output, icImage *filter, std::string capt, CvScalar color) {
	// threshold the of filtered image
	cvThreshold(filter->Image, filter->Image, 250.0, 255.0, CV_THRESH_BINARY);	
	
	CvSeq* seq = 0;
	
	//Memory allocated for OpenCV function operations
	CvMemStorage* storage = cvCreateMemStorage(0);
	cvClearMemStorage(storage);
	
	//Find connected pixel sequences within a binary OpenGL image (diff), starting at the top-left corner (0,0)
	cvFindContours(filter->Image, storage, &seq, sizeof(CvContour)*2, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE); //CV_CHAIN_APPROX_NONE

	CvFont font;
	double hScale=0.25;
	double vScale=0.2;
	int    lineWidth=1;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, hScale, vScale, 0, lineWidth);
	
	//Iterate through segments
	for(unsigned int boxNr = 0; seq; boxNr++, seq = seq->h_next) {
		//Find minimal bounding box for each sequence
		CvRect boundbox = cvBoundingRect(seq);
		
		// do not do it on the small ones
		if( boundbox.height * boundbox.width < 250 ) break;

		CvPoint p1, p2;
		p1.x = boundbox.x; p1.y = boundbox.y;
		p2.x = boundbox.x + boundbox.width; p2.y = boundbox.y + boundbox.height;
		
		// draw box
		cvRectangle(output->Image, p1, p2, color, 2, 8, 0 );

		// draw text
		char s[100];
		sprintf(s, "%s:[%d]", capt.c_str(), boxNr);
		//p1.x += 0;
		p1.y -= 3; 
		cvPutText(output->Image, s, p1, &font, cvScalar(255,0,0));
		
	}
	
	cvReleaseMemStorage(&storage);
}


/*
* Message handler. RPC and from the terminal
*/
bool CoreModule::respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply)
{
	printDebug("Message Received: (echo is on)");
	
	// debug
	printf("GOT MESSAGE: %s\n", command.toString().c_str());
	
	
	// QUIT
	if( command.get(0).asString() == "quit" ||
	    command.get(0).asString() == "exit" || 
	    command.get(0).asString() == "stop" )
	{
		isRunning = false;
		return false;
	}
	
	reply.clear();
	reply.addString("The command is not valid! Try: quit|list|add|del");	

	// nothing there
	if( command.size() < 1 ) return true;
	if( command.size() == 1 && command.get(0).asString() == "") return true;


	// LIST
	if( command.get(0).asString() == "list" || command.get(0).asString() == "ls" ){
		
		reply.clear();
		reply.addString("list");
		
		std::vector<ModuleInfo>::iterator itr;
		for ( itr = listOfModules.begin(); itr != listOfModules.end(); ++itr ) {
			std::cout << (*itr).toStdString() << std::endl;

			Bottle b;
			(*itr).toBottle(b);
			
			reply.addList() = b;
		}
		
	}
	
	// ADDING A MODULE
	if( command.get(0).asString() == "add" || command.get(0).asString() == "launch" ){
		reply.clear();		
		
		if( command.size() < 2 ) {
			reply.addString("ERROR: The syntax should be:");
			reply.addString("add <name>");	
			return true;
		} 
		
		int thisModuleID = nextModuleID++;
		
		addModuleToList(thisModuleID, command);
		
		reply.addString("OK");
		reply.addInt(thisModuleID);
	}

	// DELETING A MODULE
	if(command.get(0).asString() == "del" ||
	   command.get(0).asString() == "rm"  ||
	   command.get(0).asString() == "delete") {
		reply.clear();		
		
		if( command.size() < 2 ) {
			reply.addString("ERROR: The syntax should be:");
			reply.addString("del <moduleID>");	
			return true;
		} 
		
	    if( command.get(1).isInt() ) {
			int thisModuleID = command.get(1).asInt();
			reply.addString("OK");
			// delete from vector
			remModuleFromList(thisModuleID);

	    } else {
			reply.addString("ERROR: Could not parse integer! the syntax should be: del <moduleID as integer>");
		}
		
	}
	
//	if( command.get(0).asString() == "set"){
//		
//		if( command.get(1).asString() == "tgt" || command.get(1).asString() == "target" ) {
//			userSetTargetName = command.get(2).asString();
//			reply.clear();	
//			if( userSetTargetName == "" ) {
//				reply.addString("using latest object in the world as target!");
//			} else {
//				reply.addString(userSetTargetName.c_str());					
//				reply.addString(" is now the target!");	
//			}
//
//
//		} else 
//			
//		if( command.get(1).asString() == "offset" ) {
//			if(reach) {
//				if( command.get(2).isDouble() ) {
//					reach->setOffset( command.get(2).asDouble() );
//					reply.clear();
//					reply.addString("OK");			
//				} else {
//					reply.clear();
//					reply.addString("ERROR: Could not parse double! the syntax should be: set offset <double>");	
//				}
//			} else {
//				reply.clear();
//				reply.addString("ReachingWorker not ready yet!");	
//			}
//		} else 
//		
//		if( command.get(1).asString() == "policy" ) {
//			if(reach) {
//				
//				if( command.get(2).asString() == "above" ) {
//					reach->setPolicy( ReachingWorker::FROM_ABOVE | ReachingWorker::STRAIGHT );
//					reply.clear();
//					reply.addString("OK");
//					
//				} else if( command.get(2).asString() == "below" ) {
//					reach->setPolicy( ReachingWorker::FROM_BELOW | ReachingWorker::STRAIGHT );
//					reply.clear();
//					reply.addString("OK");	
//					
//				} else if( command.get(2).asString() == "left" ) {
//					reach->setPolicy( ReachingWorker::FROM_LEFT  | ReachingWorker::STRAIGHT );
//					reply.clear();
//					reply.addString("OK");	
//					
//				} else if( command.get(2).asString() == "right" ) {
//					reach->setPolicy( ReachingWorker::FROM_RIGHT | ReachingWorker::STRAIGHT );
//					reply.clear();
//					reply.addString("OK");	
//					
//				} else {
//					reply.clear();
//					reply.addString("ERROR: Could not parse policy! the syntax should be: set policy <above|below|left|right>");	
//				}
//			} else {
//				reply.clear();
//				reply.addString("ReachingWorker not ready yet!");	
//			}
//		} else {
//			
//			// not a correct command?!
//			reply.clear();
//			reply.addString("Could not parse command try set [tgt|policy|offset]!");	
//		}

	return true;
}


void CoreModule::addModuleToList(int ID, const Bottle &cmd) {
	ModuleInfo i;
	i.set(ID, cmd);
	
	// connect to it's rawoutput port...
	icFilterPort *p = createAndConnectPort(nextModuleID-1, i.portname.c_str());
	if(p) i.setPort(p, i.portname.c_str());
	
	// add to list
	listOfModules.push_back(i);	
}

icFilterPort* CoreModule::createAndConnectPort(int modID, const char *inputPortName) {
	icFilterPort *port = new icFilterPort(modID);
	if( port->openAndConnectPort(inputPortName) ) {
		return port;
	} else {
		port->close();
		delete port;
	}		
	return NULL;
}

void CoreModule::remModuleFromList(int thisModuleID) {
	std::vector<ModuleInfo>::iterator itr;
	for ( itr = listOfModules.begin(); itr != listOfModules.end(); ++itr ) {
		if(thisModuleID == (*itr).ID) {
			yarp.disconnect((*itr).portname.c_str(), ((*itr).port)->getName().c_str());
			(*itr).port->close();
			
			listOfModules.erase(itr);
			break;
		}
	}
}




/*
* Interrupt function.
*/
bool CoreModule::interruptModule()
{
	std::cout << "Interrupt of the module received, waiting for clean up! " << std::endl;
	leftInPort.interrupt();
//	rightInPort.interrupt();	
	outputPort.interrupt();
	return true;
}


/*
* Close function, to perform cleanup.
*/
bool CoreModule::close()
{
	std::cout << "Closing and cleaning up the module!" << std::endl;

	leftInPort.close();
//	rightInPort.close();
	outputPort.close();
	
	return true;
}

