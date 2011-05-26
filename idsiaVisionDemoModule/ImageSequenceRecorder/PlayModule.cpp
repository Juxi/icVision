/*
 * PlayModule.cpp
 *
 *  Created on: May 26, 2011
 *      Author: Davide Migliore
 * 	GPL goes here...
 */

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

  //printf("[%d] updateModule\n", count);
  cout<<"["<<framecounter<<" frame]"<< "... ";

  //Use YARP port to get images
  getImageLeft =  ( imageInLeft.getInputCount() > 0 );
  getImageRight = ( imageInRight.getInputCount() > 0 );

  if ( getImageLeft + getImageRight < 1){
    Time::delay(0.1);
    cout<<"not available"<<endl;
  }
  else{

    //Time::delay(0.3);

    iCubleft = imageInLeft.read();  // read an image
    iCubright = imageInRight.read();  // read an image

    imageLeft = Mat((IplImage*)iCubleft->getIplImage(),false);
    imageRight = Mat((IplImage*)iCubright->getIplImage(),false);

    if (cam2save == BOTH || cam2save == LEFT){
        saveImage(imageLeft, dir4left, "left", framecounter, params);
    }
    if (cam2save == BOTH || cam2save == RIGHT){
        saveImage(imageLeft, dir4right, "right", framecounter, params);
    }
    cout<<"recorded"<<endl;
    framecounter++;

  }


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
  //handlerPort.open("/IMCLEVERVision");
  //attach(handlerPort);
  //attachTerminal();

    cam2save = BOTH;

    /* Process all parameters from both command-line and .ini file */
    moduleName   = "SequenceRecorder"; //(rf.find("moduleName")).toString();
    /*
     * attach a port of the same name as the module (prefixed with a /) to the module
     * so that messages received from the port are redirected to the respond method
     */

    //where to save the images
    dir4left = "/home/icub/Desktop/SaveFolder/";
    dir4right = "/home/icub/Desktop/SaveFolder/";

    params.push_back(CV_IMWRITE_PXM_BINARY);

    cout<< moduleName <<endl;
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

    Network::connect("/icub/cam/left",inputPortNameLeft.c_str());
    Network::connect("/icub/cam/right",inputPortNameRight.c_str());

}

bool PlayModule::interruptModule(){

  cout<<"Interrupting your module, for port cleanup"<<endl;
  imageInLeft.interrupt();
  imageInRight.interrupt();
  handlerPort.interrupt();

  return true;
}

bool PlayModule::open(Searchable& config){ //remember to set the object up.
  //TODO change the config setting for customizable configuration (ref Taio code)

    /* prepare and configure the resource finder */
     ResourceFinder rf;
     rf.setVerbose( true );
     rf.setDefaultConfigFile( "SequenceRecorder.ini" ); //overridden by --from parameter
     rf.setDefaultContext( "SequenceRecorder/conf" );   //overridden by --context parameter
     rf.configure( "ICUB_ROOT", 0, NULL );
     //rf.configure( "../home/migliore/Dropbox/Projects/YarpObjectLocalization/bin/", 0, NULL );

     cout<<"Start module"<<endl;


     if( !configure(rf) ){
       return false;
     }
     return true;
}

bool PlayModule::close(){

  cout<<"Calling close function\n";
  imageInLeft.close();
  imageInRight.close();
  handlerPort.close();

  return true;
}

bool PlayModule::saveImage(Mat& image2save, string directory, string camera, int framecounter, vector<int> & params){
  char numberImage[6];
  stringstream filename;
  sprintf(numberImage, "%.06d", framecounter);
  filename<<directory<<camera<<numberImage<<".ppm";

  return imwrite(filename.str(), image2save ,params);
}
