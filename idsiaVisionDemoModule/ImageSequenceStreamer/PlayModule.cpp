/************************************************************
 * Copyright: (C) 2011 Juxi Leitner	                    	*
 * Author: Juxi Leitner <juxi.leitner@gmail.com> @ IDSIA.ch *
 *  based on code for the recordmodule by : Davide Migliore	*
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.*
 ************************************************************/

#include "PlayModule.h"

PlayModule::PlayModule()
{
  // TODO Auto-generated constructor stub

}

PlayModule::~PlayModule()
{
  // TODO Auto-generated destructor stub
}

double PlayModule::getPeriod(){

  return 1; //module periodicity (seconds)

}

bool PlayModule::updateModule(){
	cout<<"["<<framecounter<<" frame]"<< "... ";

	// prepare ports images
	yarp::sig::ImageOf<yarp::sig::PixelBgr> &imageLeftTmp = imageOutLeft.prepare();
	yarp::sig::ImageOf<yarp::sig::PixelBgr> &imageRightTmp = imageOutRight.prepare();	
	
	if (cam2play == BOTH || cam2play == LEFT){						
		// read image from hdd
		readImage(imageLeft, dir4left, "left", framecounter);
		
		if(!imageLeft.empty()) {
			//Send output images ... just place to outputImageLeft or outputImageRight what do you want to see
			imageLeftTmp = imageOutLeft.prepare();
			
			imageLeftTmp.resize(imageLeft.cols, imageLeft.rows);
			
			IplImage image_iplLeftimage = (IplImage) imageLeft;
			
			cvCopy( &image_iplLeftimage, (IplImage *) imageLeftTmp.getIplImage());
			
			imageOutLeft.write();
			cout<< "left streamed " << endl;
			
		} else std::cout << "empty image... ?! " << std::endl;
	}
	
	if (cam2play == BOTH || cam2play == RIGHT){						
		// read image from hdd
		readImage(imageRight, dir4right, "right", framecounter);
		
		if(!imageRight.empty()) {
			//Send output images ... just place to outputImageLeft or outputImageRight what do you want to see
			imageRightTmp = imageOutRight.prepare();
			
			imageRightTmp.resize(imageRight.cols, imageRight.rows);
		
			IplImage image_iplRightimage = (IplImage) imageRight;
			
			cvCopy( &image_iplRightimage, (IplImage *) imageRightTmp.getIplImage());
			
			imageOutRight.write();
			cout<< "right streamed " << endl;			
			
		} else std::cout << "empty image... ?! " << std::endl;
	}

	framecounter++;
	return true;
}

bool PlayModule::respond(const Bottle& command, Bottle& reply){

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

bool PlayModule::configure(yarp::os::ResourceFinder &rf){
    framecounter = 0;
    cam2play = BOTH;

    /* Process all parameters from both command-line and .ini file */
    moduleName   = "SequenceStreamer"; //(rf.find("moduleName")).toString();
    /*
     * attach a port of the same name as the module (prefixed with a /) to the module
     * so that messages received from the port are redirected to the respond method
     */

    // where to save the images
    dir4left = "/Users/juxi/Desktop/SaveFolder/";
    dir4right = "/Users/juxi/Desktop/SaveFolder/";
//    dir4left = "/home/icub/Desktop/SaveFolder/";
//    dir4right = "/home/icub/Desktop/SaveFolder/";

    params.push_back(CV_IMWRITE_PXM_BINARY);

    cout << moduleName << endl;
    handlerPortName =  "/";
	// use getName() rather than a literal
    handlerPortName += getName();         

    if (!handlerPort.open(handlerPortName.c_str())) {
       cout << getName() << ": Unable to open port " << handlerPortName << endl;
       return false;
    }

	// attach to handler port
    attach(handlerPort);

    //set-up input/output ports
    outputPortNameLeft = "/" + moduleName + "/left/image:o";
    if(! imageOutLeft.open( outputPortNameLeft.c_str() )){
		return false;
    }

    outputPortNameRight = "/" + moduleName + "/right/image:o";
    if(! imageOutRight.open( outputPortNameRight.c_str() )){
		return false;
    }
	
	return true;
}

bool PlayModule::interruptModule(){

  cout<<"Interrupting your module, for port cleanup"<<endl;
  imageOutLeft.interrupt();
  imageOutRight.interrupt();
  handlerPort.interrupt();

  return true;
}

bool PlayModule::open(Searchable& config){ //remember to set the object up.
  //TODO change the config setting for customizable configuration (ref Taio code)

    /* prepare and configure the resource finder */
     ResourceFinder rf;
     rf.setVerbose( true );
     rf.setDefaultConfigFile( "SequenceStreamer.ini" ); //overridden by --from parameter
     rf.setDefaultContext( "SequenceStreamer/conf" );   //overridden by --context parameter
     rf.configure( "ICUB_ROOT", 0, NULL );
     //rf.configure( "../home/migliore/Dropbox/Projects/YarpObjectLocalization/bin/", 0, NULL );

     cout << "Start module" << endl;


     if( !configure(rf) ){
       return false;
     }
     return true;
}

bool PlayModule::close(){
  cout<<"Calling close function\n";
  imageOutLeft.close();
  imageOutRight.close();
  handlerPort.close();

  return true;
}

bool PlayModule::readImage(Mat& image2save, string directory, string camera, int framecounter){
  char numberImage[6];
  stringstream filename;
  sprintf(numberImage, "%.06d", framecounter);
  filename<<directory<<camera<<numberImage<<".ppm";
  image2save = imread(filename.str());
	return true;//!(image2save == NULL);
}
